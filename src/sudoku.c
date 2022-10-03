#include "sudoku.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <getopt.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>

#define MAX_GRID_SIZE 64

char *help_msg = "Usage:  sudoku [-a| -o FILE| -v| -V| -h] FILE...\n"
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

/**
 * Return
 *  + true: if `grid_size` is belongs to possible grids size.
 *  + false: else
 */
bool is_given_grid_size_acceptable(int grid_size) {
  int possible_sizes[] = {1, 4, 9, 16, 25, 36, 49, 64};

  for (int i = 0; i < 8; i++) {
    if (grid_size == possible_sizes[i]) {
      return true;
    }
  }
  return false;
}

/**
 * Writes the `grid` in the file descriptor `fd`.
 */
static void grid_print(const grid_t *grid, FILE *fd) {

  for (size_t i = 0; i < grid->size; i++) {
    for (size_t j = 0; j < grid->size; j++) {
      fprintf(fd, "%c ", grid->cells[i][j]);
    }
    fprintf(fd, "\n");
  }
}
/**
 * Depending of the `grid` size, checks if char `c` satisfy conditions
 */
static bool check_char(const grid_t *grid, const char c) {

  bool res = false;

  switch (grid->size) {

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
    break;
  }

  return res;
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

  bool is_comment_line = false;         /** if it's an comment line*/
  bool first_row_readed = false;        /** if the first row of sudoku
                                          is already readed */
  bool any_sudoku_char_read_yet = true; /** Bool to check if any sudoku
                                          char is not read yet on a line*/

  int grid_size = 0;
  int nb_column_grid = 0;
  int nb_row_grid = 1;

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

      if (!is_comment_line && !any_sudoku_char_read_yet && !first_row_readed) {
        first_row_readed = true;

        if (!is_given_grid_size_acceptable(grid_size)) {
          warnx("error: invalid grid size '%d'.\n"
                "Possible sizes: 1, 4, 9, 16, 25, 36, 49, 64.\n",
                grid_size);

          return NULL;
        }

        grid = grid_alloc((size_t)grid_size);

        for (int i = 0; i < grid_size; i++) {

          if (check_char(grid, first_row[i])) {
            grid->cells[0][i] = first_row[i];
          } else {
            warnx("error: wrong character '%c' at line %d!\n", first_row[i],
                  nb_row_grid);

            return NULL;
          }
        }
      } else {

        if ((0 < nb_column_grid) && (nb_column_grid < grid_size)) {
          warnx("error: line %d is malformed! "
                "Grid has %d missing column(s)\n",
                nb_row_grid, grid_size - nb_column_grid);

          return NULL;
        }

        nb_column_grid = 0;
      }

      is_comment_line = false;
      any_sudoku_char_read_yet = true;
      break;

    default:

      if (!is_comment_line) {

        any_sudoku_char_read_yet = false;

        if (!first_row_readed) {
          first_row[grid_size] = c;
          grid_size++;

          break;
        } else {
          nb_column_grid++;

          if (nb_column_grid == 1) {
            nb_row_grid++;
          }

          if (nb_row_grid > grid_size) {
            warnx("error: grid has more than expected number of line (%d)\n",
                  grid_size);

            return NULL;
          }

          if (nb_column_grid > grid_size) {
            warnx("error: line %d is malformed! (More number of column)\n"
                  "Number of column got: %d\nExpected number of column: %d\n",
                  nb_row_grid, nb_column_grid, grid_size);

            return NULL;
          }

          if (check_char(grid, c)) {
            grid->cells[nb_row_grid - 1][nb_column_grid - 1] = c;
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
 * Allocate and return a pointer to an grid_t struct of size*size cells.
 */
static grid_t *grid_alloc(size_t size) {

  grid_t *grid = malloc(sizeof(grid_t));

  if (grid == NULL) {
    errx(EXIT_FAILURE, "error: Error while allocating grid structure");
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof(char *));

  for (size_t i = 0; i < size; i++) {
    grid->cells[i] = malloc(size * sizeof(char));
  }

  return grid;
}

/**
 * Free the memory of the struct `grid`
 */
static void grid_free(grid_t *grid) {
  for (size_t i = 0; i < grid->size; i++) {
    free(grid->cells[i]);
  }
  free(grid->cells);
}

int main(int argc, char *argv[]) {

  static bool verbose, unique, all, generate = false;
  static bool solver = true;

  int grid_size = 9;

  FILE *program_output = stdout;

  static struct option long_opts[] = {
      {"all", 0, 0, 'a'},     {"generate", 2, 0, 'g'},
      {"unique", 0, 0, 'u'},  {"output", 1, 0, 'o'},
      {"verbose", 0, 0, 'v'}, {"version", 0, 0, 'V'},
      {"help", 0, 0, 'h'},    {0, 0, 0, 0}};

  int optc;
  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) != -1) {
    switch (optc) {
    case 'h':
      fputs(help_msg, stdout);
      exit(EXIT_SUCCESS);

    case 'V':
      fprintf(stdout, "sudoku %d.%d.%d\n", VERSION, SUBVERSION, REVISION);
      fputs("Solve or generate sudoku grids "
            "(possible sizes: 1, 4, 9, 16, 25, 36, 49, 64)",
            stdout);
      exit(EXIT_SUCCESS);

    case 'v':
      verbose = true;
      break;

    case 'o':
      FILE *file = fopen(optarg, "w+");
      if (file == NULL) {
        errx(EXIT_FAILURE, "error: Error while opening file %s", optarg);
      }

      program_output = file;
      // fclose(file);
      break;

    case 'a':
      all = true;
      break;

    case 'g':
      generate = true;

      if (optarg) {
        grid_size = atoi(optarg);

        if (!is_given_grid_size_acceptable(grid_size)) {
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

    return 0;
  }

  if (unique) {
    unique = false;
    warnx("warning: option 'unique' conflict with solver mode, disabling it!");
  };

  if (optind == argc) {
    errx(EXIT_FAILURE, "error: no input grid given!");
  }

  /* Check if files are readable */
  for (int i = optind; i < argc; i++) {

    if (access(argv[i], R_OK) != 0) {
      errx(EXIT_FAILURE, "error: file '%s' is not readeable!", argv[i]);
    }
  }

  fprintf(program_output, "---Solveur mode---\n");

  bool is_one_grid_valid = false; /** At least, if one grid is valid */

  for (int i = optind; i < argc; i++) {

    fprintf(program_output, "------Grid %d--------\n", i - optind + 1);

    grid_t *grid = file_parser(argv[i]);

    if (grid != NULL) {
      is_one_grid_valid = true;
      grid_print(grid, program_output);
    }

    fprintf(program_output, "-------------------\n");
  }

  if (!is_one_grid_valid) {
    errx(EXIT_FAILURE, "error: Any grid is valid. Nothing to solve!");
  }

  return 0;
}