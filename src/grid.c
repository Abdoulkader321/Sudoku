#include "grid.h"
#include <colors.h>

/* Internal structure (hiden from outside) to represent a sudoku grid */
struct _grid_t {
  size_t size;
  colors_t **cells;
};

int main(void) { return EXIT_SUCCESS; }