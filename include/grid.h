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

typedef struct choice_t choice_t;

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

/* Return True if grid is consistent, False otherwise */
bool grid_is_consistent(grid_t *grid);

/**
 * Apply heuristics on grid and return:
 * + 0: if the grid is not solved but still consistent
 * + 1: if the grid is solved
 * + 2: if the grid is inconsistent
 * */
size_t grid_heuristics(grid_t *grid, bool use_locked_candidates);

/* Free a choice_t data structure */
void grid_choice_free(choice_t *choice);

/* Check if the color set of the choice is empty or not */
bool grid_choice_is_empty(const choice_t *choice);

/* Apply the choice to the given grid */
void grid_choice_apply(grid_t *grid, const choice_t *choice);

/* Blank (set to full colors) the given choice */
void grid_choice_blank(grid_t *grid, const choice_t *choice);

/* Discard the choice from the grid */
void grid_choice_discard(grid_t *grid, const choice_t *choice);

/* Display the choice on the file descriptor */
void grid_choice_print(const choice_t *choice, FILE *fd);

/* Return a choice of the smallest set of colors, NULL otherwise */
choice_t *grid_choice(grid_t *grid);

/* Do a deep copy of grid_b in grid_a */
void grid_deep_copy(grid_t *grid_a, grid_t *grid_b);

/* Return a new grid of specified size containing full colors except one cell */
grid_t *get_new_grid(const size_t size);

/* Remove randomly specified number of colors in the grid. Remove means to put
 * full colors.*/
void remove_some_colors(grid_t *grid, size_t nb_colors_to_remove);

/* Remove randomly one color in the grid and return it. The cell from which the
 * color is removed must not be in 'tab'. For example, the cell [2][3] is stored
 * in 'tab' like this: 23 (2*10 + 3).
 */
choice_t *remove_one_color(grid_t *grid, int *tab, size_t tab_size);

#endif /* GRID_H */