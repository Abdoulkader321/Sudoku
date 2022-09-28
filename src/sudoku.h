#ifndef SUDOKU_H
#define SUDOKU_H

#define VERSION 1
#define SUBVERSION 0
#define REVISION 0

#include <stdlib.h>
#include <stdio.h>

typedef struct{
  size_t size;
  char** cells;
} grid_t;

static grid_t* grid_alloc (size_t size);
static void grid_free (grid_t * grid);

static void grid_print (const grid_t* grid, FILE* fd);

#endif /* SUDOKU_H */