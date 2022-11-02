#include "colors.h"

#include <stdio.h>

#include <math.h>
#include <time.h>

#define ULONG_MAX 0xffffffffffffffff

colors_t colors_full(const size_t size) {

  return (size >= MAX_COLORS) ? ULONG_MAX : (1UL << size) - 1;
}

colors_t colors_empty(void) { return 0; }

colors_t colors_set(const size_t color_id) {

  return color_id >= MAX_COLORS ? 0 : 1UL << color_id;
}

colors_t colors_add(const colors_t colors, const size_t color_id) {

  return colors | colors_set(color_id);
}

colors_t colors_discard(const colors_t colors, const size_t color_id) {

  return colors == 0 ? 0 : (colors & ~colors_set(color_id));
}

colors_t colors_discard_B_from_A(const colors_t colorsA,
                                 const colors_t colorsB) {

  return colorsA + colorsB == 0 ? 0 : (colorsA & colors_negate(colorsB));
}

bool colors_is_in(const colors_t colors, const size_t color_id) {

  return (colors & colors_set(color_id)) != 0;
}

colors_t colors_negate(const colors_t colors) {

  return colors_xor(colors, ULONG_MAX);
}

colors_t colors_and(const colors_t colors1, const colors_t colors2) {

  return colors1 & colors2;
}

colors_t colors_or(const colors_t colors1, const colors_t colors2) {

  return colors1 | colors2;
}

colors_t colors_xor(const colors_t colors1, const colors_t colors2) {

  return colors1 ^ colors2;
}

colors_t colors_is_equal(const colors_t colors1, const colors_t colors2) {

  return colors1 == colors2;
}

bool colors_is_singleton(const colors_t colors) {

  return colors == 0 ? false : (colors & (colors - 1)) == 0;
}

size_t colors_count(const colors_t colors) {

  colors_t colors_count = colors;

  /* In binary, 0x5555555555555555 looks like ..0101 */
  colors_count = colors_count - ((colors_count >> 1) & 0x5555555555555555);

  /* 0x3333333333333333 looks like ..0011 */
  colors_count = (colors_count & 0x3333333333333333) +
                 ((colors_count >> 2) & 0x3333333333333333);

  /* 0x0F0F0F0F0F0F0F0F looks like ..00001111 */
  /* 0x0101010101010101 looks like ..000000001 */
  colors_count = (((colors_count + (colors_count >> 4)) & 0x0F0F0F0F0F0F0F0F) *
                  0x0101010101010101) >>
                 56;

  return colors_count;
}

bool colors_is_subset(const colors_t colors1, const colors_t colors2) {

  return colors1 == 0 ? 0 : (colors2 | colors1) == colors2;
}

colors_t colors_subtract(const colors_t colors1, const colors_t colors2) {

  return (colors1 ^ colors2) & colors1;
}

colors_t colors_rightmost(const colors_t colors) {

  return colors & colors_negate(colors - 1);
}

colors_t colors_leftmost(const colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t nb_colors = colors_count(colors);
  size_t nb_colors_found = 0;
  size_t pos = 0;

  while (nb_colors_found < nb_colors) {

    if (colors_is_in(colors, pos)) {
      nb_colors_found++;
    }

    pos++;
  }
  return colors_set(pos - 1);
}

colors_t colors_random(const colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t nb_colors = colors_count(colors);

  srand(time(NULL));
  size_t random_color = (rand() % nb_colors) + 1;

  size_t pos = 0;

  while (random_color != 0) {

    if (colors_is_in(colors, pos)) {
      random_color--;
    }

    pos++;
  }

  return colors_set(pos - 1);
}

/** Return
 *  + True if 'cross-hatching' heuristic could be applied on subgrid
 *  + False otherwise
 */
static bool cross_hatching(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  for (size_t i = 0; i < size; i++) {

    if (colors_is_singleton(*subgrid[i])) {

      colors_t singleton_color = *subgrid[i];

      for (size_t j = 0; j < size; j++) {

        if (!colors_is_singleton(*subgrid[j])) {

          colors_t cell_colors = *subgrid[j];

          colors_t discard =
              colors_discard_B_from_A(cell_colors, singleton_color);

          if (!colors_is_equal(cell_colors, discard)) {
            *subgrid[j] = discard;
            subgrid_changed = true;
          }
        }
      }
    }
  }

  return subgrid_changed;
}

/** Return
 *  + True if 'lone-number' heuristic could be applied on subgrid
 *  + False otherwise
 */
static bool lone_number(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  for (size_t i = 0; i < size; i++) {
    /* Checking the occurences of each color of subgrid */

    colors_t color = colors_set(i);
    size_t color_occurence = 0;
    int color_index = 0;

    for (size_t j = 0; j < size; j++) {
      colors_t cell_colors = *subgrid[j];

      if (colors_is_in(cell_colors, i)) {
        color_occurence++;
        color_index = j;
      }

      if (color_occurence >= 2) {
        break;
      }
    }

    if (color_occurence == 1 && !colors_is_singleton(*subgrid[color_index])) {
      *subgrid[color_index] = color;
      subgrid_changed = true;
    }
  }

  return subgrid_changed;
}

static bool naked_hidden_subset(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  for (size_t N = 2; N < ceil(size / 2); N++) {

    for (size_t i = 0; i < size; i++) {

      if (colors_count(*subgrid[i]) == N) {

        size_t nb_cells_with_N_candidates = 0;
        size_t nb_cells_with_no_N_candidates = 0;

        size_t index_cells_with_N_candidates[size];
        size_t index_cells_with_no_N_candidates[size];

        for (size_t j = 0; j < size; j++) {

          if (!colors_is_singleton(*subgrid[j]) &&
              colors_is_subset(*subgrid[j], *subgrid[i])) {

            index_cells_with_N_candidates[nb_cells_with_N_candidates] = j;
            nb_cells_with_N_candidates++;
          } else {
            index_cells_with_no_N_candidates[nb_cells_with_no_N_candidates] = j;
            nb_cells_with_no_N_candidates++;
          }
        }

        if (nb_cells_with_N_candidates == N) {
          // naked N found

          /* Naked subset */
          for (size_t k = 0; k < nb_cells_with_no_N_candidates; k++) {

            colors_t other_cell_colors =
                *subgrid[index_cells_with_no_N_candidates[k]];

            colors_t new_other_cell_colors =
                colors_discard_B_from_A(other_cell_colors, *subgrid[i]);

            if (new_other_cell_colors != other_cell_colors) {

              *subgrid[index_cells_with_no_N_candidates[k]] =
                  new_other_cell_colors;
              subgrid_changed = true;
            }
          }

          /* Hidden subset */
          for (size_t k = 0; k < nb_cells_with_N_candidates; k++) {

            colors_t candidates_cell_colors =
                *subgrid[index_cells_with_N_candidates[k]];

            colors_t candidates_cell_new_colors =
                colors_and(candidates_cell_colors, *subgrid[i]);

            if (candidates_cell_colors != candidates_cell_new_colors) {

              candidates_cell_colors = candidates_cell_new_colors;
              subgrid_changed = true;
            }
          }
        }
      }
    }
  }

  return subgrid_changed;
}

bool subgrid_heuristics(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  subgrid_changed |= cross_hatching(subgrid, size);

  subgrid_changed |= lone_number(subgrid, size);

  subgrid_changed |= naked_hidden_subset(subgrid, size);

  return subgrid_changed;
}
