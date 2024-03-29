#include "grid.h"

#include <colors.h>

#include <math.h>
#include <string.h>
#include <time.h>

#define status_code_grid_is_not_solved_and_consistent 0
#define status_code_grid_is_solved 1
#define status_code_grid_is_inconsistent 2

static bool seed_intialized = false;

/* Internal structure (hiden from outside) to represent a sudoku grid */
struct _grid_t {
  size_t size;
  colors_t **cells;
};

struct choice_t {
  size_t row;
  size_t column;
  colors_t color;
};

void grid_print(const grid_t *grid, FILE *fd) {
  size_t grid_size = grid_get_size(grid);

  for (size_t i = 0; i < grid_size; i++) {
    for (size_t j = 0; j < grid_size; j++) {

      char *colors = grid_get_cell(grid, i, j);

      if (colors == NULL) {
        return;
      }

      if (strlen(colors) == grid_size) {

        fprintf(fd, "%c", (grid_size == 1) ? color_table[0] : EMPTY_CELL);

      } else {
        fprintf(fd, "%s", colors);
      }

      fprintf(fd, " ");
      free(colors);
    }
    fprintf(fd, "\n");
  }
}

bool grid_check_char(const grid_t *grid, const char c) {

  bool res = false;

  switch (grid_get_size(grid)) {

  case 64:
    res |= ('n' <= c && c <= 'z') || (c == '*') || (c == '&');
    // FALL THROUGH

  case 49:
    res |= ('a' <= c && c <= 'm');
    // FALL THROUGH

  case 36:
    res |= ('Q' <= c && c <= 'Z') || (c == '@');
    // FALL THROUGH

  case 25:
    res |= ('H' <= c && c <= 'P');
    // FALL THROUGH

  case 16:
    res |= ('A' <= c && c <= 'G');
    // FALL THROUGH

  case 9:
    res |= ('5' <= c && c <= '9');
    // FALL THROUGH

  case 4:
    res |= ('2' <= c && c <= '4');
    // FALL THROUGH

  case 1:
    res |= (c == '1') || (c == '_');
    // FALL THROUGH
    break;
  }

  return res;
}

grid_t *grid_alloc(size_t size) {

  if (!grid_check_size(size)) {
    return NULL;
  }

  grid_t *grid = malloc(sizeof(grid_t));
  if (grid == NULL) {
    return NULL;
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof(colors_t *));
  if (grid->cells == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < size; i++) {

    grid->cells[i] = malloc(size * sizeof(colors_t));
    if (grid->cells[i] == NULL) {
      return NULL;
    }
  }

  return grid;
}

void grid_free(grid_t *grid) {

  if (grid == NULL) {
    return;
  }

  for (size_t i = 0; i < grid_get_size(grid); i++) {
    free(grid->cells[i]);
  }

  free(grid->cells);
  free(grid);
}

bool grid_check_size(const size_t size) {

  return (size == 1) || (size == 4) || (size == 9) || (size == 16) ||
         (size == 25) || (size == 36) || (size == 49) || (size == 64);
}

grid_t *grid_copy(const grid_t *grid) {

  if (grid == NULL) {
    return NULL;
  }

  grid_t *grid_copy;

  grid_copy = grid_alloc(grid->size);
  if (grid_copy == NULL)
    return NULL;

  for (size_t i = 0; i < grid->size; i++) {
    for (size_t j = 0; j < grid->size; j++) {
      grid_copy->cells[i][j] = grid->cells[i][j];
    }
  }

  return grid_copy;
}

void grid_deep_copy(grid_t *grid_a, grid_t *grid_b) {
  size_t size = grid_a->size;
  if (size != grid_b->size) {
    return;
  }

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      grid_a->cells[i][j] = grid_b->cells[i][j];
    }
  }
}

size_t grid_get_size(const grid_t *grid) {

  return grid == NULL ? 0 : grid->size;
}

/* Return the color associated to a given character*/
static colors_t char2color(const char color, size_t grid_size) {

  if (color == EMPTY_CELL) {
    return colors_full(grid_size);
  }

  size_t i;

  for (i = 0; i < MAX_GRID_SIZE; i++) {
    if (color_table[i] == color) {
      break;
    }
  }

  return colors_set(i);
}

/* Return as a string all the colors contained in `colors` */
static char *colors2string(const colors_t colors, size_t grid_size) {

  size_t nb_colors = colors_count(colors);

  char *colors_string = malloc(sizeof(char) * (nb_colors + 1));
  if (colors_string == NULL) {
    return NULL;
  }

  size_t index_colors = 0;
  for (size_t i = 0; i < grid_size; i++) {
    if (colors_is_in(colors, i)) {
      colors_string[index_colors] = color_table[i];
      index_colors++;
    }
  }
  colors_string[nb_colors] = '\0';

  return colors_string;
}

void grid_set_cell(const grid_t *grid, const size_t row, const size_t column,
                   const char color) {

  if ((grid == NULL) || (row >= grid->size) || (column >= grid->size)) {
    return;
  }

  grid->cells[row][column] = char2color(color, grid->size);
}

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column) {

  if ((grid == NULL) || (row >= grid->size) || (column >= grid->size)) {
    return NULL;
  }

  return colors2string(grid->cells[row][column], grid->size);
}

/* Return the square root of 'size' */
static size_t get_sqrt(const size_t size) {

  switch (size) {
  case 64:
    return 8;

  case 49:
    return 7;

  case 36:
    return 6;

  case 25:
    return 5;

  case 16:
    return 4;

  case 9:
    return 3;

  case 4:
    return 2;

  case 1:
    return 1;

  default:
    return 0;
  }
}

/* Return True if the subgrid is consistent, False otherwise */
static bool subgrid_consistency(colors_t *subgrid[], const size_t size) {

  colors_t subgrid_colors = 0;

  for (size_t i = 0; i < size; i++) {

    colors_t color = *subgrid[i];

    if (color == 0) {
      return false;
    }

    if (colors_is_singleton(color)) {

      colors_t xor = colors_xor(subgrid_colors, color);

      if (xor < subgrid_colors) {
        return false;
      }

      subgrid_colors = xor;
    }
  }

  subgrid_colors = 0;
  for (size_t i = 0; i < size; i++) {
    subgrid_colors = colors_or(subgrid_colors, *subgrid[i]);
  }

  return subgrid_colors == colors_full(size);
}

bool grid_is_consistent(grid_t *grid) {

  colors_t *subgrid[grid->size];
  size_t index;

  for (size_t row = 0; row < grid->size; row++) {
    index = 0;

    for (size_t column = 0; column < grid->size; column++) {
      subgrid[index] = &grid->cells[row][column];
      index++;
    }

    if (!subgrid_consistency(subgrid, grid->size)) {
      return false;
    }
  }

  for (size_t column = 0; column < grid->size; column++) {
    index = 0;

    for (size_t row = 0; row < grid->size; row++) {
      subgrid[index] = &grid->cells[row][column];
      index++;
    }

    if (!subgrid_consistency(subgrid, grid->size)) {
      return false;
    }
  }

  size_t size_sqrt = get_sqrt(grid->size);

  for (size_t block = 0; block < grid->size; block++) {
    index = 0;
    size_t row_start = ((block / size_sqrt) * size_sqrt);

    for (size_t row = row_start; row < size_sqrt + row_start; row++) {
      size_t column_start = ((block % size_sqrt) * size_sqrt);

      for (size_t column = column_start; column < size_sqrt + column_start;
           column++) {
        subgrid[index] = &grid->cells[row][column];
        index++;
      }
    }

    if (!subgrid_consistency(subgrid, grid->size)) {
      return false;
    }
  }

  return true;
}

static bool grid_is_solved(grid_t *grid) {

  for (size_t i = 0; i < grid->size; i++) {

    for (size_t j = 0; j < grid->size; j++) {

      if (!colors_is_singleton(grid->cells[i][j])) {
        return false;
      }
    }
  }

  return true;
}

/* Remove specified colors from specified row. Colors between specified columns
 * will not be removed */
static bool remove_colors_from_row(grid_t *grid, colors_t colors_to_remove,
                                   size_t row, size_t column_excluded_start,
                                   size_t column_excluded_end) {

  bool changed = false;

  for (size_t column = 0; column < grid->size; column++) {

    if (column < column_excluded_start || column > column_excluded_end) {

      colors_t *cell_colors = &grid->cells[row][column];
      colors_t colors_removed_from_cell =
          colors_discard_B_from_A(*cell_colors, colors_to_remove);

      if (!colors_is_singleton(*cell_colors) &&
          colors_removed_from_cell != *cell_colors) {
        changed = true;
        *cell_colors = colors_removed_from_cell;
      }
    }
  }
  return changed;
}

/* Remove specified colors from specified column. Colors in specified rows
 * will not be removed */
static bool remove_colors_from_column(grid_t *grid, colors_t colors_to_remove,
                                      size_t column, size_t row_excluded_start,
                                      size_t row_excluded_end) {

  bool changed = false;

  for (size_t row = 0; row < grid->size; row++) {

    if (row < row_excluded_start || row > row_excluded_end) {

      colors_t *cell_colors = &grid->cells[row][column];
      colors_t colors_removed_from_cell =
          colors_discard_B_from_A(*cell_colors, colors_to_remove);

      if (!colors_is_singleton(*cell_colors) &&
          colors_removed_from_cell != *cell_colors) {
        changed = true;
        *cell_colors = colors_removed_from_cell;
      }
    }
  }
  return changed;
}

/** Return
 *  + True if 'locked_candidates' heuristic could be applied on subgrid
 *  + False otherwise
 */
static bool subgrid_locked_candidates(grid_t *grid, colors_t *subgrid[],
                                      size_t block_start_row,
                                      size_t block_start_column) {

  /* locked candidates on row */;
  bool changed = false;
  size_t size_sqrt = get_sqrt(grid->size);
  colors_t row_colors[size_sqrt]; /* colors in the same row of subgrid*/
  size_t index = 0;

  changed |= cross_hatching(subgrid, grid->size);

  for (size_t i = 0; i < grid->size; i += size_sqrt) {
    colors_t colors = 0;
    for (size_t j = 0; j < size_sqrt; j++) {
      if (!colors_is_singleton(*subgrid[i + j])) {
        colors = colors_or(colors, *subgrid[i + j]);
      }
    }
    row_colors[index] = colors;
    index++;
  }

  for (size_t row = 0; row < size_sqrt; row++) {
    colors_t colors = 0;
    for (size_t j = 0; j < size_sqrt; j++) {

      if (row != j) {
        colors = colors_or(colors, row_colors[j]);
      }
    }
    colors_t colors_to_remove =
        colors_and(colors_negate(colors), row_colors[row]);
    if (colors_to_remove != 0) {
      changed |= remove_colors_from_row(
          grid, colors_to_remove, row + block_start_row, block_start_column,
          block_start_column + size_sqrt - 1);
    }
  }

  /* locked candidates on column */;

  colors_t column_colors[size_sqrt]; /*colors in the same column of subgrid*/
  index = 0;

  for (size_t i = 0; i < size_sqrt; i++) {
    colors_t colors = 0;
    for (size_t j = 0; j < grid->size; j += size_sqrt) {
      if (!colors_is_singleton(*subgrid[i + j])) {
        colors = colors_or(colors, *subgrid[i + j]);
      }
    }
    column_colors[index] = colors;
    index++;
  }

  for (size_t column = 0; column < size_sqrt; column++) {
    colors_t colors = 0;
    for (size_t j = 0; j < size_sqrt; j++) {

      if (column != j) {
        colors = colors_or(colors, column_colors[j]);
      }
    }
    colors_t colors_to_remove =
        colors_and(colors_negate(colors), column_colors[column]);
    if (colors_to_remove != 0) {
      changed |= remove_colors_from_column(
          grid, colors_to_remove, column + block_start_column, block_start_row,
          block_start_row + size_sqrt - 1);
    }
  }

  return changed;
}

size_t grid_heuristics(grid_t *grid, bool use_locked_candidates) {

  bool is_fixpoint_not_reached = true;

  while (is_fixpoint_not_reached) {

    is_fixpoint_not_reached = false;
    colors_t *subgrid[grid->size];
    size_t index;

    for (size_t row = 0; row < grid->size; row++) {
      index = 0;

      for (size_t column = 0; column < grid->size; column++) {
        subgrid[index] = &grid->cells[row][column];
        index++;
      }

      is_fixpoint_not_reached |= subgrid_heuristics(subgrid, grid->size);
      if (!subgrid_consistency(subgrid, grid->size)) {
        return status_code_grid_is_inconsistent;
      }
    }

    for (size_t column = 0; column < grid->size; column++) {
      index = 0;

      for (size_t row = 0; row < grid->size; row++) {
        subgrid[index] = &grid->cells[row][column];
        index++;
      }

      is_fixpoint_not_reached |= subgrid_heuristics(subgrid, grid->size);
      if (!subgrid_consistency(subgrid, grid->size)) {
        return status_code_grid_is_inconsistent;
      }
    }

    size_t size_sqrt = get_sqrt(grid->size);

    for (size_t block = 0; block < grid->size; block++) {
      index = 0;
      size_t row_start = ((block / size_sqrt) * size_sqrt);

      for (size_t row = row_start; row < size_sqrt + row_start; row++) {
        size_t column_start = ((block % size_sqrt) * size_sqrt);

        for (size_t column = column_start; column < size_sqrt + column_start;
             column++) {
          subgrid[index] = &grid->cells[row][column];
          index++;
        }
      }

      is_fixpoint_not_reached |= subgrid_heuristics(subgrid, grid->size);
      if (!subgrid_consistency(subgrid, grid->size)) {
        return status_code_grid_is_inconsistent;
      }
    }

    if (use_locked_candidates && !is_fixpoint_not_reached) {

      for (size_t block = 0; block < grid->size; block++) {
        colors_t *subgrid[grid->size];
        size_t index = 0;
        size_t row_start = ((block / size_sqrt) * size_sqrt);
        size_t column_start = ((block % size_sqrt) * size_sqrt);

        for (size_t row = row_start; row < size_sqrt + row_start; row++) {

          for (size_t column = column_start; column < size_sqrt + column_start;
               column++) {

            subgrid[index] = &grid->cells[row][column];
            index++;
          }
        }

        is_fixpoint_not_reached |=
            subgrid_locked_candidates(grid, subgrid, row_start, column_start);

        if (block % size_sqrt == 0 && !grid_is_consistent(grid)) {
          return status_code_grid_is_inconsistent;
        }
      }
    }
  }

  return grid_is_solved(grid) ? status_code_grid_is_solved
                              : status_code_grid_is_not_solved_and_consistent;
}

void grid_choice_free(choice_t *choice) { free(choice); }

bool grid_choice_is_empty(const choice_t *choice) { return choice->color == 0; }

void grid_choice_apply(grid_t *grid, const choice_t *choice) {

  grid->cells[choice->row][choice->column] = choice->color;
}

void grid_choice_discard(grid_t *grid, const choice_t *choice) {

  colors_t new_colors =
      colors_discard_B_from_A(grid->cells[choice->row][choice->column],
                              choice->color); /* The choice is discarded */

  grid->cells[choice->row][choice->column] = new_colors;
}

void grid_choice_print(const choice_t *choice, FILE *fd) {

  for (size_t i = 0; i < MAX_GRID_SIZE; i++) {

    if (colors_is_in(choice->color, i)) {
      fprintf(fd, "Grid[%ld][%ld]'s choice is '%c'.\n", choice->row,
              choice->column, color_table[i]);
      return;
    }
  }
}

choice_t *grid_choice(grid_t *grid) {

  size_t choice_row = 0;
  size_t choice_column = 0;
  size_t choice_cell_length = grid->size;

  for (size_t row = 0; row < grid->size; row++) {
    for (size_t column = 0; column < grid->size; column++) {

      size_t tmp = colors_count(grid->cells[row][column]);
      if (tmp > 1 && tmp < choice_cell_length) {
        choice_row = row;
        choice_column = column;
        choice_cell_length = tmp;
      }
    }
  }

  if (choice_cell_length > 1) {
    choice_t *choice = malloc(sizeof(choice_t));
    if (choice == NULL) {
      return NULL;
    }

    choice->row = choice_row;
    choice->column = choice_column;
    choice->color = colors_rightmost(grid->cells[choice_row][choice_column]);

    return choice;
  }
  return NULL;
}

grid_t *get_new_grid(const size_t size) {

  grid_t *grid = grid_alloc(size);
  grid->size = size;
  colors_t all_colors = colors_full(size);

  if (!seed_intialized) {
    srand(time(NULL));
    seed_intialized = true;
  }

  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {

      grid->cells[i][j] = all_colors;
    }
  }

  size_t index_i = rand() % size;
  size_t index_j = (index_i * 2) % size;
  colors_t random = colors_set(index_i);
  grid->cells[index_i][index_j] = random;

  return grid;
}

void remove_some_colors(grid_t *grid, size_t nb_colors_to_remove) {

  size_t size = grid->size;
  size_t nb_colors_to_remove_per_line = ceil(nb_colors_to_remove / size);
  colors_t full_colors = colors_full(size);

  if (!seed_intialized) {
    srand(time(NULL));
    seed_intialized = true;
  }

  for (size_t i = 0; i < size; i++) {

    for (size_t j = 0; j < nb_colors_to_remove_per_line; j++) {
      size_t index = rand() % size;
      grid->cells[i][index] = full_colors;
    }
  }
}

choice_t *remove_one_color(grid_t *grid, int *tab, size_t tab_size) {

  bool is_finished = false;
  choice_t *choice = malloc(sizeof(choice_t));
  if (choice == NULL) {
    return NULL;
  }
  if (!seed_intialized) {
    srand(time(NULL));
    seed_intialized = true;
  }
  size_t row = 0;
  size_t column = 0;
  while (!is_finished) {
    row = rand() % grid->size;
    column = rand() % grid->size;
    int tmp = row * 10 + column;
    bool is_in_tab = false;

    for (size_t i = 0; i < tab_size; i++) {
      if (tab[i] == tmp) {
        is_in_tab = true;
        break;
      }
    }

    is_finished = !is_in_tab && colors_is_singleton(grid->cells[row][column]);
  }

  choice->row = row;
  choice->column = column;
  choice->color = grid->cells[row][column];
  grid->cells[row][column] = colors_full(grid->size);
  return choice;
}