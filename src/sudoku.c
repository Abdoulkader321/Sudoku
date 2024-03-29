#include "sudoku.h"

#include "grid.c"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <assert.h>
#include <err.h>
#include <getopt.h>
#include <string.h>

#define GRID_DEFAULT_SIZE 9
#define EMPTY_CELLS_RATE 0.4

static bool verbose = false;
static size_t count_solved_grid = 0;

/* Return a grid structure that contains the first row of input grid */
static grid_t *write_first_row_to_grid(char *first_row, int grid_size) {

  grid_t *grid;

  if (!grid_check_size(grid_size)) {
    warnx("error: invalid grid size '%d'.\n"
          "Possible sizes: 1, 4, 9, 16, 25, 36, 49, 64.\n",
          grid_size);

    return NULL;
  }

  grid = grid_alloc((size_t)grid_size);
  if (grid == NULL) {
    errx(EXIT_FAILURE, "error: Error while allocating grid structure");
  }

  for (int i = 0; i < grid_size; i++) {

    if (grid_check_char(grid, first_row[i])) {
      grid_set_cell(grid, 0, i, first_row[i]);
    } else {
      warnx("error: wrong character '%c' at line 1!\n", first_row[i]);
      return NULL;
    }
  }

  return grid;
}

/**
 * This parser returns:
 *  + a pointer to the grid if it's a valid grid in the file `filename`.
 *  + null, otherwise.
 */
static grid_t *file_parser(char *filename) {

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    errx(EXIT_FAILURE, "error: Error while opening file %s", filename);
  }

  int c;
  grid_t *grid = NULL;
  char first_row[MAX_GRID_SIZE];

  bool is_comment_line = false;
  bool first_row_readed = false;
  bool any_sudoku_char_read_yet = true; /** Bool to check if any sudoku
                                          char is not read yet on a line*/

  int grid_size = 0;
  int nb_column_grid = 0;
  int nb_row_grid = 0;

  while ((c = fgetc(file)) != EOF) {

    switch (c) {
    case '#':
      is_comment_line = true;
      break;

    case ' ':
    case '\t':
      break;

    case '\n':

      if (!any_sudoku_char_read_yet) {

        if (!first_row_readed) {
          first_row_readed = true;
          grid = write_first_row_to_grid(first_row, grid_size);

          if (grid == NULL) {
            return NULL;
          }
        } else {

          if (nb_column_grid < grid_size) {
            warnx("error: line %d is malformed! "
                  "Grid has %d missing column(s)\n",
                  nb_row_grid, grid_size - nb_column_grid);

            return NULL;
          }
        }

        nb_column_grid = 0;
      }

      is_comment_line = false;
      any_sudoku_char_read_yet = true;
      break;

    default:

      if (!is_comment_line) {

        any_sudoku_char_read_yet = false;

        nb_column_grid++;

        if (nb_column_grid == 1) {
          nb_row_grid++;
        }

        if (!first_row_readed) {
          first_row[grid_size] = c;
          grid_size++;
          break;

        } else {

          if (nb_row_grid > grid_size) {
            warnx("error: grid has %d line(s) more than expected.\n",
                  nb_row_grid - grid_size);
            return NULL;
          }

          if (nb_column_grid > grid_size) {
            warnx("error: grid has %d column(s) more than expected.\n",
                  nb_column_grid - grid_size);
            return NULL;
          }

          if (grid_check_char(grid, c)) {
            grid_set_cell(grid, nb_row_grid - 1, nb_column_grid - 1, c);
          } else {
            warnx("error: wrong character '%c' at line %d column %d!\n", c,
                  nb_row_grid, nb_column_grid);
            return NULL;
          }
        }
      }
      break;
    }
  }
  fclose(file);

  if ((nb_row_grid == 1) && (nb_column_grid != 0)) {
    /** This happens when the first line do not end with `\n`*/

    grid = write_first_row_to_grid(first_row, grid_size);

    if (grid == NULL) {
      return NULL;
    }
  }

  if (grid_size == 0) {
    warnx("error: grid is empty");

    return NULL;
  }

  if (nb_row_grid != grid_size) {
    warnx("error: grid has %d missing line(s)", grid_size - nb_row_grid);

    return NULL;
  }

  if (nb_column_grid > 0 && nb_column_grid != grid_size) {
    /** when a non empty line ends with EOF */

    warnx("error: grid has %d missing column(s)", grid_size - nb_column_grid);

    return NULL;
  }

  return grid;
}

/**
 * Return:
 * + 0: if the grid is not solved but still consistent
 * + 1: if the grid is solved and display it
 * + 2: if the grid is inconsistent
 */
static size_t grid_solver(grid_t *grid, const mode_t mode, FILE *fd) {

  grid_t *grid_cpy;
  choice_t *choice;

  size_t res = grid_heuristics(grid, true);

  switch (res) {

  case 1:
    count_solved_grid++;
    fflush(fd);

    if (mode == mode_all) {
      fprintf(fd, "Solution %lu\n", count_solved_grid);
    } else {
      fprintf(fd, "Solution\n");
    }
    grid_print(grid, fd);
    // FALL THROUGH

  case 2:
    return res;

  case 0:

    grid_cpy = grid_copy(grid);
    if (grid_cpy == NULL) {
      errx(EXIT_FAILURE, "error: Error while doing a deep code of grid\n");
    }

    choice = grid_choice(grid_cpy);
    assert(choice != NULL);

    grid_choice_apply(grid_cpy, choice);
    size_t backtracking_res = grid_solver(grid_cpy, mode, fd);
    grid_free(grid_cpy);

    if (backtracking_res == 1 && mode == mode_first) {
      grid_choice_free(choice);
      return 1;
    }

    grid_choice_discard(grid, choice);
    grid_choice_free(choice);

    return grid_solver(grid, mode, fd);

  default:
    return res;
  }
}

/**
 * Return:
 * + 0: if the grid is not solved but still consistent
 * + 1: if the grid is solved and display it
 * + 2: if the grid is inconsistent
 */
static size_t grid_solver_for_generator(grid_t *grid, const generator_t mode) {

  grid_t *grid_cpy;
  choice_t *choice;
  size_t res = grid_heuristics(grid, false);

  switch (res) {

  case 1:
    count_solved_grid++;
    // FALL THROUGH

  case 2:
    return res;

  case 0:

    grid_cpy = grid_copy(grid);
    if (grid_cpy == NULL) {
      errx(EXIT_FAILURE, "error: Error while doing a deep code of grid\n");
    }

    choice = grid_choice(grid_cpy);
    assert(choice != NULL);

    grid_choice_apply(grid_cpy, choice);
    size_t backtracking_res = grid_solver_for_generator(grid_cpy, mode);

    if (backtracking_res == 1) {
      bool is_finished = false;

      if (mode == mode_not_unique) {
        is_finished = true;
      } else if ((mode == mode_unique) && count_solved_grid >= 2) {
        is_finished = true;
      }

      if (is_finished) {
        grid_deep_copy(grid, grid_cpy);
        grid_free(grid_cpy);
        grid_choice_free(choice);
        return backtracking_res;
      }
    }

    grid_free(grid_cpy);
    grid_choice_discard(grid, choice);
    grid_choice_free(choice);

    return grid_solver_for_generator(grid, mode);

  default:
    return res;
  }
}

/* Generate a grid of specified size */
static grid_t *grid_generator(const bool is_unique_mode, const size_t size) {

  grid_t *grid = get_new_grid(size);
  generator_t mode = is_unique_mode ? mode_unique : mode_not_unique;
  grid_solver_for_generator(grid, mode);

  if (!is_unique_mode) {
    size_t nb_colors_to_remove = ceil(size * size * EMPTY_CELLS_RATE);
    remove_some_colors(grid, nb_colors_to_remove);
  } else {

    int tab[grid->size]; /* Contains index of cells from which colors must not
                            be removed */
    size_t index = 0;
    size_t nb_color_removed = 0;
    size_t nb_color_to_remove = grid->size * grid->size * EMPTY_CELLS_RATE;

    while (nb_color_removed < nb_color_to_remove) {

      choice_t *choice = remove_one_color(grid, tab, index);
      grid_t *grid_cpy = grid_copy(grid);
      if (grid_cpy == NULL) {
        errx(EXIT_FAILURE, "error: Error while doing a deep code of grid\n");
      }

      count_solved_grid = 0;
      grid_solver_for_generator(grid_cpy, mode);

      if (count_solved_grid == 1) {
        nb_color_removed++;

      } else {
        grid->cells[choice->row][choice->column] = choice->color;
        tab[index] = choice->column + 10 * choice->row;
        index++;

        nb_color_removed--;
      }
      grid_choice_free(choice);
      grid_free(grid_cpy);
    }
  }

  return grid;
}

int main(int argc, char *argv[]) {

  const char *help_msg =
      "Usage:  sudoku [-a| -o FILE| -v| -V| -h] FILE...\n"
      "\tsudoku -g[SIZE] [-u| -o FILE| -v| -V| -h]\n"
      "Solve or generate Sudoku grids of various sizes "
      "(1, 4, 9, 16, 25, 36, 49, 64)\n\n"
      "-a, -all\t\t search for all possible solutions\n"
      "-g[N], --generate[=N]\t generate a grid of size N*N "
      "(default:9)\n"
      "-u, --unique\t\t generate a grid with unique solution\n"
      "-o FILE, --output FILE\t write solution to File\n"
      "-v, --verbose\t\t verbose output\n"
      "-V, --version\t\t display version and exit\n"
      "-h, --help\t\t display this help and exit";

  bool unique = false;
  bool all = false;
  bool generate = false;

  int grid_size = GRID_DEFAULT_SIZE;

  FILE *program_output = stdout;
  char *output_file_name = NULL;

  const struct option long_opts[] = {{"all", no_argument, NULL, 'a'},
                                     {"generate", optional_argument, NULL, 'g'},
                                     {"unique", no_argument, NULL, 'u'},
                                     {"output", required_argument, NULL, 'o'},
                                     {"verbose", no_argument, NULL, 'v'},
                                     {"version", no_argument, NULL, 'V'},
                                     {"help", no_argument, NULL, 'h'},
                                     {NULL, no_argument, NULL, no_argument}};

  int optc;
  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) !=
         -1) {

    switch (optc) {
    case 'h':
      fputs(help_msg, stdout);
      exit(EXIT_SUCCESS);

    case 'V':
      fprintf(stdout, "sudoku %d.%d.%d\n", VERSION, SUBVERSION, REVISION);
      fputs("Solve or generate sudoku grids "
            "(possible sizes: 1, 4, 9, 16, 25, 36, 49, 64)\n",
            stdout);
      exit(EXIT_SUCCESS);

    case 'v':
      verbose = true;
      break;

    case 'o':
      output_file_name = optarg;

      break;

    case 'a':
      all = true;
      break;

    case 'g':
      generate = true;

      if (optarg) {
        grid_size = atoi(optarg);

        if (!grid_check_size(grid_size)) {
          errx(EXIT_FAILURE,
               "error: invalid grid size '%d'. \n"
               "Possible sizes: 1, 4, 9, 16, 25, 36, 49, 64.",
               grid_size);
        }
      }
      break;

    case 'u':
      unique = true;
      break;

    default:
      errx(EXIT_FAILURE, "error: invalid option '%s'\nCheck './sudoku -h' !",
           argv[optind - 1]);
    }
  }

  if (output_file_name != NULL) {
    program_output = fopen(output_file_name, "w+");
  }

  if (program_output == NULL) {
    errx(EXIT_FAILURE, "error: Error while opening file %s", optarg);
  }

  if (generate) {
    fprintf(program_output, "# Generator mode \n");

    grid_t *grid = grid_generator(unique, grid_size);
    grid_print(grid, program_output);
    grid_free(grid);
    return EXIT_SUCCESS;
  }

  if (unique) {
    unique = false;
    warnx("warning: option 'unique' conflict with solver mode, disabling it!");
  }

  if (optind == argc) {
    errx(EXIT_FAILURE, "error: no input grid given!");
  }

  /* Check if files are readable */
  for (int i = optind; i < argc; i++) {

    if (access(argv[i], R_OK) != 0) {
      errx(EXIT_FAILURE, "error: file '%s' is not readeable or do not exist!",
           argv[i]);
    }
  }

  fprintf(program_output, "---Solveur mode---\n");

  bool are_all_grids_consistent = true;

  for (int i = optind; i < argc; i++) {

    fprintf(program_output, "------Grid %d: %s--------\n", i - optind + 1,
            argv[i]);

    grid_t *grid = file_parser(argv[i]);

    if ((grid != NULL) && grid_is_consistent(grid)) {

      count_solved_grid = 0;
      grid_solver(grid, all ? mode_all : mode_first, program_output);
      grid_free(grid);

      if (count_solved_grid != 0) {

        fprintf(program_output, "# Number of solutions: %ld\n",
                count_solved_grid);
        fprintf(program_output, "The grid is solved!\n");
        are_all_grids_consistent &= true;

      } else {
        warnx("The grid is inconsistent!\n");
        are_all_grids_consistent &= false;
      }

    } else {

      warnx("Initial grid is inconsistent or not valid\n");
      are_all_grids_consistent &= false;
      grid_free(grid);
    }

    fprintf(program_output, "-------------------\n");
  }

  fclose(program_output);

  return are_all_grids_consistent ? EXIT_SUCCESS : EXIT_FAILURE;
}