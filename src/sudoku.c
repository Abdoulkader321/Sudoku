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

char* help_msg= "Usage:  sudoku [-a| -o FILE| -v| -V| -h] FILE...\n"
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


static void grid_print (const grid_t* grid, FILE* fd){

  for(size_t i = 0; i < grid->size; i++){
    for(size_t j = 0; j < grid->size; j++){
      fprintf(fd, "%c ", grid->cells[i][j]);
    }
    fprintf(fd, "\n");
  }
}

static bool check_char (const grid_t* grid, const char c){

  switch (grid->size){
    case 1:
      return (c == '1') || (c == '_');
      break;

    case 4:
      return ('1' <= c && c <= '4') || (c == '_');
      break;
    
    case 9:
      return ('1' <= c && c <= '9') || (c == '_');
      break;

    case 16:
      return ('1' <= c && c <= '9') || ('A' <= c && c <= 'G') || (c == '_');
      break;

    case 25:
      return ('1' <= c && c <= '9') || ('A' <= c && c <= 'P') || (c == '_');
      break;

    case 36:
      return ('1' <= c && c <= '9') || ('A' <= c && c <= 'Z') || (c == '@') 
        || (c == '_');
      break;
    
    case 49:
      return ('1' <= c && c <= '9') || ('A' <= c && c <= 'Z') 
          || ('a' <= c && c <= 'm') || (c == '@') ||(c == '_');
      break;
    
    case 64:
      return ('1' <= c && c <= '9') || ('A' <= c && c <= 'Z') || 
        ('a' <= c && c <= 'z') || (c == '@') || (c == '*') || (c == '&') 
        ||(c == '_');
      break;

    default:
      break;
  }

  return true;
}

static grid_t* file_parser (char* filename){

  FILE* file;
  file = fopen(filename, "r");
  
  if (file == NULL){
    errx (EXIT_FAILURE, "error: Error while opening file %s", filename);
  }

  char c;
  int grid_size = 0;
  grid_t* grid = NULL;

  bool is_comment_line = false; /** if it is an comment line*/
  char first_row[MAX_GRID_SIZE];
  bool first_row_readed = false; /** if the first row is already readed */

  int nb_column_grid = 0;
  int nb_row_grid = 1;

  while ((c = fgetc(file)) != EOF){

    switch (c){
      case '#':
        is_comment_line = true;
        break;

      case ' ':
        break;

      case '\t':
        break;
      
      case '\n':

        if (!is_comment_line && !first_row_readed){
          first_row_readed = true;

          grid = grid_alloc((size_t) grid_size);

          for(int i = 0; i< grid_size; i++){
            
            if (check_char(grid, first_row[i])){
              grid->cells[0][i] = first_row[i];
            }else{
              warnx ("error: wrong character '%c' at line %d!\n", 
                      first_row[i], nb_row_grid);
              
              return NULL;
            }
          
          }

        }else {

          if ((0 < nb_column_grid) && (nb_column_grid < grid_size)){
            warnx ("error: line %d is malformed! "
            "Grid has %d missing column(s)\n", grid_size, grid_size - nb_column_grid);
            
            return NULL;
          }

          nb_column_grid = 0;

        }

        is_comment_line = false;
        break;
      
      default:
      
        if(!is_comment_line){
          
          if(!first_row_readed){
            first_row[grid_size] = c;
            grid_size++;
            
            break;
          }else{
            nb_column_grid++;

            if(nb_column_grid == 1){
              nb_row_grid++;
            }

            if (nb_row_grid > grid_size){
              warnx ("error: grid has more than expected number of line (%d)\n", 
                    grid_size);
              
              return NULL;
            }

            if (nb_column_grid > grid_size){
              warnx ("error: line %d is malformed! (More number of column)\n"
                "Number of column got: %d\nExpected number of column: %d\n",
                nb_row_grid, nb_column_grid, grid_size);
              
              return NULL;
            }
            
            if (check_char(grid, c)){
              grid->cells[nb_row_grid - 1][nb_column_grid - 1] = c;
            }else{
              warnx ("error: wrong character '%c' at line %d column %d!\n", 
                c, nb_row_grid, nb_column_grid);
              
              return NULL;
            }
          }
        }
        break;
    }

  }
  fclose(file);
  
  if (nb_row_grid != grid_size){
    warnx ("error: grid has %d missing line(s)", 
      grid_size - nb_row_grid);
    
    return NULL;
  }
  
  return grid;
}

static grid_t* grid_alloc (size_t size){
  
  grid_t* grid = malloc(sizeof (grid_t));
  
  if (grid == NULL){
    errx (EXIT_FAILURE, "error: Error while allocating grid structure");
  }

  grid->size = size;
  grid->cells = malloc(size * sizeof (char*));


  for(size_t i = 0; i < size; i++){
    grid->cells[i] = malloc(size * sizeof (char));
  }

  return grid;
}

static void grid_free (grid_t * grid){
  for(size_t i = 0; i < grid->size; i++){
    free(grid->cells[i]);
  }
  free(grid->cells);
}

void test_malloc(int grid_size){
  grid_t* grid = grid_alloc(grid_size);

  for(int i = 0; i < grid_size; i++){
    for(int j = 0; j < grid_size; j++){
      
      grid->cells[i][j] = (char) j+1;
    }
  }
  grid_print(grid, stdout);

  printf("check_char %d", check_char(grid, '*'));
}

/**
 * Return 
 *  + true: if `grid_size` is belongs to possible grids size. 
 *  + false: else
 * 
*/
bool is_given_grid_size_acceptable(int grid_size){
	int possible_sizes[] = {1, 4, 9, 16, 25, 36, 49, 64}; 

	for (int i = 0; i < 8; i++){
		if (grid_size == possible_sizes[i]){
			return true;
		}
	}
  return false;
}


int main(int argc, char* argv[]){
  
  static bool verbose, unique, all, generate = false;
  static bool solver = true; 

  int grid_size = 9;

  static struct option long_opts[] = {
      {"all", 0, 0, 'a'},
      {"generate", 2, 0, 'g'},
      {"unique", 0, 0, 'u'},
      {"output", 1, 0, 'o'},
      {"verbose", 0, 0, 'v'},
      {"version", 0, 0, 'V'},
      {"help", 0, 0, 'h'},
      {0, 0, 0, 0}
  };

  int optc;
  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) != -1){
    switch (optc){
      case 'h':
        fputs(help_msg, stdout);
	      exit(EXIT_SUCCESS);

      case 'V':
        fprintf(stdout, "sudoku %d.%d.%d\n", VERSION, SUBVERSION, REVISION);
        fputs("Solve or generate sudoku grids "
              "(possible sizes: 1, 4, 9, 16, 25, 36, 49, 64)", stdout);
	      exit(EXIT_SUCCESS);

      case 'v':
        verbose = true;
        break;
      
      case 'o':
        FILE* file = fopen(optarg, "w+");
        if (file == NULL){
          errx (EXIT_FAILURE, "error: Error while opening file %s", optarg);
        }
        fclose(file);
        break;
      
      case 'a':
        all = true;
        break;

      case 'g':
        generate = true;

        if (optarg){
          grid_size = atoi(optarg);

          if (!is_given_grid_size_acceptable(grid_size)){
            errx (EXIT_FAILURE, 
            "error: invalid grid size '%d'. \n"
            "Possible sizes: 1, 4, 9, 16, 25, 36, 49, 64.", grid_size);
          }
        }
        break;

      case 'u':
        unique = true;
        break;

      default:
        errx (EXIT_FAILURE, "error: invalid option '%s'\nCheck './sudoku -h' !", 
          argv[optind -1]);
    }
  }

  if (generate){
    fprintf(stdout, "---Generator mode---\n");
    solver = false;


    return 0;
  }

  if (unique){
    unique = false;
    warnx("warning: option 'unique' conflict with solver mode, disabling it!");
  };

  if (optind == argc){
    errx (EXIT_FAILURE, "error: no input grid given!");
  }

  /* Check if files are readable */
  for (int i = optind; i < argc; i++){

    if (access(argv[i], R_OK) != 0){
      errx (EXIT_FAILURE, "error: file '%s' is not readeable!", argv[i]);
    }

  }

  fprintf(stdout, "---Solveur mode---\n");

  for (int i = optind; i < argc; i++){
    
    fprintf(stdout, "---Grid %d: %s---\n", i - optind + 1, argv[i]);

    grid_t* grid = file_parser(argv[i]);

    if(grid != NULL){
      grid_print(grid, stdout);
    }

    fprintf(stdout, "-----------\n");
  }

  return 0;
}