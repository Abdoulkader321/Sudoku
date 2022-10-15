#include "sudoku.h"
#include "grid.c"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <err.h>
#include <getopt.h>
#include <string.h>

#define MAX_GRID_SIZE 64

static bool verbose = false;

/**
 * Return a grid that contains the first row of input grid
 */
static grid_t *write_first_row_to_grid(char *first_row, int grid_size) {

  grid_t *grid;

  if (!grid_check_size(grid_size)) {
    warnx("error: invalid grid size '%d'.\n"
          "Possible sizes: 1, 4, 9, 16, 25, 36, 49, 64.\n",
          grid_size);

    return NULL;
  }
  grid = grid_alloc((size_t)grid_size);

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
 *  + a pointer to the grid if it's a valid grid.
 *  + null, otherwise.
 */
static grid_t *file_parser(char *filename) {

  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    errx(EXIT_FAILURE, "error: Error while opening file %s", filename);
  }

  char c;
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
      break;

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
  bool solver = true;

  int grid_size = 9;

  FILE *program_output = stdout;

  const struct option long_opts[] = {
      {"all", no_argument, 0, 'a'},     {"generate", optional_argument, 0, 'g'},
      {"unique", no_argument, 0, 'u'},  {"output", required_argument, 0, 'o'},
      {"verbose", no_argument, 0, 'v'}, {"version", no_argument, 0, 'V'},
      {"help", no_argument, 0, 'h'},    {NULL, no_argument, NULL, 0}};

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
      program_output = fopen(optarg, "w+");

      if (program_output == NULL) {
        errx(EXIT_FAILURE, "error: Error while opening file %s", optarg);
      }

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

  if (generate) {
    fprintf(program_output, "---Generator mode---\n");
    solver = false;

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

  bool are_all_grids_valid = true;

  for (int i = optind; i < argc; i++) {

    fprintf(program_output, "------Grid %d: %s--------\n", i - optind + 1,
            argv[i]);

    grid_t *grid = file_parser(argv[i]);

    are_all_grids_valid &= (grid != NULL);

    if (grid != NULL) {
      grid_print(grid, program_output);
      grid_free(grid);
    }

    fprintf(program_output, "-------------------\n");
  }

  fclose(program_output);

  return are_all_grids_valid ? EXIT_SUCCESS : EXIT_FAILURE;
}