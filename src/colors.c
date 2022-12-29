#include "colors.h"

#include <stdio.h>

#include "grid.h"
#include <math.h>
#include <time.h>

#define ULONG_MAX 0xffffffffffffffff

static bool seed_intialized = false;

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

  return ((colorsA == 0) || (colorsB == 0))
             ? 0
             : (colorsA & colors_negate(colorsB));
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

  return colors1 == 0 ? true : (colors2 | colors1) == colors2;
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

size_t color_index(colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t pos = 0;
  bool is_found = false;

  while (!is_found) {
    if (colors_is_in(colors, pos)) {
      is_found = true;
    }
    pos++;
  }
  return pos - 1;
}

colors_t colors_random(colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t nb_colors = colors_count(colors);

  if (!seed_intialized) {
    srand(time(NULL));
    seed_intialized = true;
  }

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
bool cross_hatching(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;
  bool is_finished = false;

  while (!is_finished) {
    is_finished = true;
    colors_t singleton_colors = 0;

    for (size_t i = 0; i < size; i++) {

      if (colors_is_singleton(*subgrid[i])) {
        singleton_colors = colors_or(singleton_colors, *subgrid[i]);
      }
    }

    for (size_t i = 0; i < size; i++) {

      if (!colors_is_singleton(*subgrid[i]) &&
          colors_and(singleton_colors, *subgrid[i]) > 0) {
        subgrid_changed = true;

        *subgrid[i] = colors_discard_B_from_A(*subgrid[i], singleton_colors);

        if (colors_is_singleton(*subgrid[i])) {
          is_finished = false;
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
bool lone_number(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;
  colors_t all_colors = colors_empty();
  colors_t common_colors = colors_empty();

  for (size_t i = 0; i < size; i++) {

    if (!colors_is_singleton(*subgrid[i])) {

      colors_t intersection = colors_and(all_colors, *subgrid[i]);
      all_colors = colors_or(all_colors, *subgrid[i]);
      common_colors = colors_or(common_colors, intersection);
    }
  }

  all_colors = colors_subtract(all_colors, common_colors);

  if (all_colors != 0) {

    for (size_t i = 0; i < size; i++) {

      if (!colors_is_singleton(*subgrid[i]) &&
          colors_and(*subgrid[i], all_colors) != 0) {

        *subgrid[i] = colors_leftmost(colors_and(*subgrid[i], all_colors));
        subgrid_changed = true;
      }
    }
  }

  return subgrid_changed;
}

static bool naked_subset(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  for (size_t i = 0; i < size; i++) {

    if (!colors_is_singleton(*subgrid[i])) {

      colors_t *tab_index[size];
      size_t index = 0;

      size_t compteur = 0;

      for (size_t j = 0; j < size; j++) {

        if (!colors_is_singleton(*subgrid[j])) {
          if (colors_is_subset(*subgrid[j], *subgrid[i])) {
            compteur++;
          } else {
            tab_index[index] = subgrid[j];
            index++;
          }
        }
      }

      if (compteur == colors_count(*subgrid[i])) {

        for (size_t j = 0; j < index; j++) {

          colors_t colors_removed =
              colors_discard_B_from_A(*tab_index[j], *subgrid[i]);

          if (!colors_is_equal(colors_removed, *tab_index[j])) {
            subgrid_changed = true;
            *tab_index[j] = colors_removed;
          }
        }
      }
    }
  }

  return subgrid_changed;
}

static bool hidden_subset(colors_t *subgrid[], size_t size) {

  bool subgrid_changed = false;

  for (size_t i = 0; i < size; i++) {

    if (!colors_is_singleton(*subgrid[i])) {

      colors_t reference_colors = *subgrid[i];

      size_t tab_index[size]; /** Contains the index of candidates */
      size_t nb_element_tab = 0;

      for (size_t j = 0; j < size; j++) {

        if (colors_and(reference_colors, *subgrid[j]) > 0) {
          tab_index[nb_element_tab] = j;
          nb_element_tab++;
        }
      }

      if (nb_element_tab == colors_count(reference_colors)) {

        for (size_t k = 0; k < nb_element_tab; k++) {

          colors_t candidates_cell_colors = *subgrid[tab_index[k]];

          colors_t candidates_cell_new_colors =
              colors_and(reference_colors, candidates_cell_colors);

          if (candidates_cell_colors != candidates_cell_new_colors) {
            *subgrid[tab_index[k]] = candidates_cell_new_colors;
            subgrid_changed = true;
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
  if (subgrid_changed) {
    subgrid_changed |= naked_subset(subgrid, size);
  }

  return subgrid_changed;
}
