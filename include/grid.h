#ifndef GRID_H
#define GRID_H

#define MAX_GRID_SIZE 64
#define EMPTY_CELL '_'

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const char color_table[] = "123456789"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "@"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "&*";

/* Sudoku grid (forward declaration to hide the implementation)*/
typedef struct _grid_t grid_t;

/* Allocate and return a pointer to an grid_t struct of size*size cells */
grid_t *grid_alloc(size_t size);

/* Free the memory of the struct `grid` */
void grid_free();

/* Depending of the `grid` size, checks if char `c` satisfy conditions */
bool grid_check_char(const grid_t *grid, const char c);

/* Return True if `size` belongs to possible grids size, False otherwise */
bool grid_check_size(const size_t size);

/* Return the size of `grid` */
size_t grid_get_size(const grid_t *grid);

/* Writes the `grid` in the file descriptor `fd`. */
void grid_print(const grid_t *grid, FILE *fd);

/* Do a Deep copy of `grid` in a new memory area and return it */
grid_t *grid_copy(const grid_t *grid);

/* Return the content of a given cell as a string */
char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column);

/* Set a grid cell to a specific value */
void grid_set_cell(const grid_t *grid, const size_t row, const size_t column,
                   const char color);

/* Return True if the grid is solved(has only singletons), False otherwise */
bool grid_is_solved(grid_t *grid);

/* Return True if grid is consistent, False otherwise */
bool grid_is_consistent(grid_t *grid);

#endif /* GRID_H */