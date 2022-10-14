CFLAGS = -std=c11 -Wall -Wextra -g -O2 -pedantic
CPPFLAGS = -I../include -DDEBUG

EXE = sudoku colors grid

all: sudoku colors grid

sudoku: sudoku.o
	$(CC) $(CFLAGS) -o $@ $< ;

sudoku.o: sudoku.c sudoku.h 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

colors: colors.o
	$(CC) $(CFLAGS) -o $@ $<;

colors.o: colors.c ../include/colors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

grid: grid.o 
	$(CC) $(CFLAGS) -o $@ $< ;

grid.o: grid.c ../include/grid.h 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

clean:
	rm -f *.o $(EXE) 

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help