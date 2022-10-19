#include "grid.h"

#include <colors.h>

#include <err.h>
#include <string.h>

/* Internal structure (hiden from outside) to represent a sudoku grid */
struct _grid_t {
  size_t size;
  colors_t **cells;
};

void grid_print(const grid_t *grid, FILE *fd) {
  size_t grid_size = grid_get_size(grid);

  for (size_t i = 0; i < grid_size; i++) {
    for (size_t j = 0; j < grid_size; j++) {

      char *colors = grid_get_cell(grid, i, j);

      if (strlen(colors) == grid_size) {
        fprintf(fd, "%c ", EMPTY_CELL);
      } else {
        fprintf(fd, "%s ", colors);
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

  default:
    errx(EXIT_FAILURE, "error: invalid grid size `%ld`.\n",
         grid_get_size(grid));
  }

  return res;
}

static void grid_alloc_msg_error() {
  errx(EXIT_FAILURE, "error: Error while allocating grid structure");
}

grid_t *grid_alloc(size_t size) {

  if (!grid_check_size(size)) {
    return NULL;
  }

  grid_t *grid = malloc(sizeof(grid_t));
  if (grid == NULL) {
    grid_alloc_msg_error();
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof(colors_t *));
  if (grid->cells == NULL) {
    grid_alloc_msg_error();
  }

  for (size_t i = 0; i < size; i++) {

    grid->cells[i] = malloc(size * sizeof(colors_t));
    if (grid->cells[i] == NULL) {
      grid_alloc_msg_error();
    }
  }

  return grid;
}

/**
 * Free the memory of the struct `grid`
 */
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
    errx(EXIT_FAILURE, "error: Error while allocating string for colors");
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