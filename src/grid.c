#include "grid.h"

#include <colors.h>

#include <math.h>
#include <string.h>

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

  size_t max_cells_colors = 0;

  for (size_t i = 0; i < grid_size; i++) {
    for (size_t j = 0; j < grid_size; j++) {
      size_t nb_colors = colors_count(grid->cells[i][j]);
      if (nb_colors > max_cells_colors) {
        max_cells_colors = nb_colors;
      }
    }
  }

  for (size_t i = 0; i < grid_size; i++) {
    for (size_t j = 0; j < grid_size; j++) {

      const char *colors = grid_get_cell(grid, i, j);
      size_t nb_colors = strlen(colors);

      if (colors == NULL) {
        return;
      }

      if (strlen(colors) == grid_size) {
        fprintf(fd, "%c", (grid_size == 1) ? color_table[0] : EMPTY_CELL);

      } else {
        fprintf(fd, "%s", colors);
      }

      for (size_t k = nb_colors; k <= max_cells_colors; k++) {
        fprintf(fd, " ");
      }

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

  size_t grid_size_sqrt = sqrt(grid->size);

  for (size_t block = 0; block < grid->size; block++) {

    index = 0;

    size_t row_start = ((block / grid_size_sqrt) * grid_size_sqrt);

    for (size_t row = row_start; row < grid_size_sqrt + row_start; row++) {

      size_t column_start = ((block % grid_size_sqrt) * grid_size_sqrt);

      for (size_t column = column_start; column < grid_size_sqrt + column_start;
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

size_t grid_heuristics(grid_t *grid) {

  bool is_fixpoint_not_reached = true;

  static const size_t status_code_grid_is_solved = 1;
  static const size_t status_code_grid_is_not_solved_and_consistent = 0;
  static const size_t status_code_grid_is_inconsistent = 2;

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

    size_t grid_size_sqrt = sqrt(grid->size);

    for (size_t block = 0; block < grid->size; block++) {

      index = 0;

      size_t row_start = ((block / grid_size_sqrt) * grid_size_sqrt);

      for (size_t row = row_start; row < grid_size_sqrt + row_start; row++) {

        size_t column_start = ((block % grid_size_sqrt) * grid_size_sqrt);

        for (size_t column = column_start;
             column < grid_size_sqrt + column_start; column++) {

          subgrid[index] = &grid->cells[row][column];
          index++;
        }
      }

      is_fixpoint_not_reached |= subgrid_heuristics(subgrid, grid->size);
      if (!subgrid_consistency(subgrid, grid->size)) {
        return status_code_grid_is_inconsistent;
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

  if (new_colors == 0) {
    printf("********debut********\n");
    grid_choice_print(choice, stdout);
    grid_print(grid, stdout);
  }

  grid->cells[choice->row][choice->column] = new_colors;

  if (grid->cells[choice->row][choice->column] == 0) {
    printf("**************\n");
    grid_print(grid, stdout);
    printf("********FIN********\n");
  }
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

  printf("********Choice******\n");
  grid_print(grid, stdout);
  printf("****\n");

  for (size_t N = 2; N < ceil(grid->size); N++) {

    for (size_t row = 0; row < grid->size; row++) {

      for (size_t column = 0; column < grid->size; column++) {

        if (colors_count(grid->cells[row][column]) == N) {

          choice_t *choice = malloc(sizeof(choice_t));

          if (choice == NULL) {
            return NULL;
          }

          choice->row = row;
          choice->column = column;
          choice->color = colors_rightmost(grid->cells[row][column]);

          printf("[%ld][%ld]\n", row, column);
          printf("********ENDCHOICE******\n");

          return choice;
        }
      }
    }
  }
  printf("Pas de choix\n********ENDCHOICE******\n");
  return NULL;
}