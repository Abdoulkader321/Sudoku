#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

int main(int argc, char* argv[]){

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
    while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) != -1)
    {
        switch (optc)
        {
        case 'h':
            fputs("Usage:  sudoku [-a| -o FILE| -v| -V| -h] FILE...\n"
                    "\tsudoku -g[SIZE] [-u| -o FILE| -v| -V| -h]\n"
                    "Solve or generate Sudoku grids of various sizes (1, 4, 9, 16, 25, 36, 49, 64)\n"
            , stdout);
            exit(EXIT_SUCCESS);

        case '?':
            fputs("Option not recognized", stdout);
            exit(EXIT_FAILURE);
        default:
            fputs("Any option set!", stdout);
            exit(EXIT_FAILURE);
        }
    }
    
    return 0;
}