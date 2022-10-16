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

void grid_print(const grid_t *grid, FILE *fd);
bool grid_check_char(const grid_t *grid, const char c);

grid_t *grid_alloc(size_t size);
void grid_free();

bool grid_check_size(const size_t size);

grid_t *grid_copy(const grid_t *grid);

size_t grid_get_size(const grid_t *grid);

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column);

void grid_set_cell(const grid_t *grid, const size_t row, const size_t column,
                   const char color);

#endif /* GRID_H */