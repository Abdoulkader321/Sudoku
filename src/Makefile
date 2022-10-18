CFLAGS = -std=c11 -Wall -Wextra -g -O2 -pedantic
CPPFLAGS = -I../include -DDEBUG
LDFLAGS=

EXE = sudoku 

all: sudoku grid.o

sudoku: sudoku.o colors.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS);

sudoku.o: sudoku.c sudoku.h ../include/grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

colors.o: colors.c ../include/colors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

grid.o: grid.c ../include/grid.h ../include/colors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

clean:
	rm -f *.o $(EXE) 

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help