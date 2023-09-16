# Sudoku

Programmer un solveur et un générateur  du fameux jeu ```sudoku```. 

Le solveur sudoku permet de résoudre des grilles de différentes tailles (4, 9, 16, 25, 36, 49, 64) en donnant une ou toutes les solutions possibles. 

Le générateur, quant à lui, permet de créer des grilles de sudoku de différentes tailles. Il peut également créer des grilles qui ont une unique solution.

Le but étant de faire en sorte que le programme soit très performant et le code de la meilleure qualité possible.

Vous pouvez trouver le rapport [ici](report/Report.pdf).

```bash
$ make

$ ./sudoku --help
Usage:  sudoku [-a| -o FILE| -v| -V| -h] FILE...
        sudoku -g[SIZE] [-u| -o FILE| -v| -V| -h]
Solve or generate Sudoku grids of various sizes (1, 4, 9, 16, 25, 36, 49, 64)

-a, -all                 search for all possible solutions
-g[N], --generate[=N]    generate a grid of size N*N (default:9)
-u, --unique             generate a grid with unique solution
-o FILE, --output FILE   write solution to File
-v, --verbose            verbose output
-V, --version            display version and exit
-h, --help               display this help and exit 

```

### `Solveur mode`: Résout des grilles de sudoku de taille 4x4, 9x9, 16x16, 25x25, 36x36, 49x49, 64x64.

**Dans le dossier 'tests/challenges', nous retrouvons quelques grilles de sudoku de différentes tailles à résoudre.**

```bash
$ ./sudoku tests/challenges/level-03/grid-09x09-01.sku 
---Solveur mode---
------Grid 1: tests/challenges/level-03/grid-09x09-01.sku--------
Solution
8 1 2 7 5 3 6 4 9 
9 4 3 6 8 2 1 7 5 
6 7 5 4 9 1 2 8 3 
1 5 4 2 3 7 8 9 6 
3 6 9 8 4 5 7 2 1 
2 8 7 1 6 9 5 3 4 
5 2 1 9 7 4 3 6 8 
4 3 8 5 2 6 9 1 7 
7 9 6 3 1 8 4 5 2 
# Number of solutions: 1
The grid is solved!
-------------------

$ ./sudoku -a tests/challenges/level-03/grid-09x09-21.sku
---Solveur mode---
------Grid 1: tests/challenges/level-03/grid-09x09-21.sku--------
Solution 1
7 8 5 1 2 6 3 4 9 
1 2 6 3 4 9 5 7 8 
3 4 9 5 7 8 1 2 6 
2 6 1 4 8 5 7 9 3 
8 7 3 9 6 1 2 5 4 
5 9 4 2 3 7 6 8 1 
4 1 7 6 9 2 8 3 5 
6 3 2 8 5 4 9 1 7 
9 5 8 7 1 3 4 6 2 
Solution 2
7 8 5 1 2 6 3 4 9 
1 2 6 3 4 9 5 7 8 
3 4 9 5 7 8 6 2 1 
2 6 1 4 8 5 7 9 3 
8 7 3 9 6 1 2 5 4 
5 9 4 2 3 7 1 8 6 
4 1 7 6 9 2 8 3 5 
6 3 2 8 5 4 9 1 7 
9 5 8 7 1 3 4 6 2 
# Number of solutions: 2
The grid is solved!
-------------------

```

### `Generator mode`: Génère des grilles de sudoku de taille 4x4, 9x9, 16x16, 25x25, 36x36, 49x49, 64x64.

```bash
$ ./sudoku --generate 9 
# Generator mode 
_ 1 5 2 _ 6 7 8 _ 
6 _ 7 5 8 9 _ _ 4 
_ _ 9 1 4 7 _ 5 6 
3 4 1 _ _ 2 9 6 _ 
_ 5 2 _ 9 8 _ 4 1 
9 6 8 3 1 _ _ 2 _ 
1 _ 3 4 6 _ 8 _ 2 
2 8 4 _ 7 3 _ 1 _ 
5 9 6 8 _ 1 _ 7 _ 
                                                                                                                                    # Génère une grille de taille 9x9 qui a une unique solution.                               
$ ./sudoku -u --generate 9 
# Generator mode 
1 _ 4 2 _ 6 7 8 9 
5 6 _ _ 8 9 1 3 4 
_ 8 _ 1 3 _ 2 _ _ 
_ _ _ 5 4 7 _ _ _ 
_ 5 _ 6 9 _ _ 1 2 
_ 9 8 _ _ 2 4 7 5 
3 2 _ 4 6 _ 8 _ _ 
8 4 6 _ 7 _ 5 _ 1 
9 7 1 8 2 _ _ _ 3 

```

```
$ make clean
```