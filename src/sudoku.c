#include "sudoku.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <err.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

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

/**
 * Return 
 *  + true: if `grid_size` is belongs to possible grids size. 
 *  + false: else
 * 
*/
bool is_given_grid_size_acceptable(int grid_size){
	int possible_sizes[] = {1, 4, 9, 16, 25, 35, 49, 64}; 

	for(int i=0; i<8; i++){
		if(grid_size == possible_sizes[i]){
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
  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL))!= -1)
  {
    switch (optc)
    {
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
        if(file == NULL){
          errx (EXIT_FAILURE, "error: Error while opening file %s", optarg);
        }
        fclose(file);
        break;
      
      case 'a':
        all = true;
        break;

      case 'g':
        generate = true;

        if(optarg){
          grid_size = atoi(optarg);

          if(!is_given_grid_size_acceptable(grid_size)){
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

  if(generate){
    fprintf(stdout, "---Generator mode---\n");
    solver = false;
    return 0;

  }

  if(unique){
    unique = false;
    warnx("warning: option 'unique' conflict with solver mode, disabling it!");
  };

  if(optind == argc){
    errx (EXIT_FAILURE, "error: no input grid given!");
  }

  // Check if files are readable
  for(int i = optind; i < argc; i++){

    if(access(argv[i], R_OK) != 0){
      errx (EXIT_FAILURE, "error: file '%s' is not readeable!", argv[i]);
    }

  }

  fprintf(stdout, "---Solveur mode---\n");

  return 0;
}