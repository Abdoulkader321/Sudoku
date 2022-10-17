CFLAGS = -std=c11 -Wall -Wextra -g -O2 -pedantic
CPPFLAGS = -I../include -DDEBUG

EXE = colors_tests grid_tests

all: colors_tests grid_tests

colors_tests: colors_tests.o colors.o
	$(CC) $(CFLAGS) -o $@ $^;

colors_tests.o: colors_tests.c ../src/colors.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

grid_tests: grid_tests.o grid.o colors.o
	$(CC) $(CFLAGS) -o $@ $^;

grid_tests.o: grid_tests.c ../src/grid.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< ;

colors.o: ../src/colors.c ../include/colors.h
	@cd ../src/ && $(MAKE)
	@cp ../src/colors.o ./

grid.o: ../src/grid.c ../include/grid.h ../include/colors.h
	@cd ../src/ && $(MAKE)
	@cp ../src/grid.o ./




clean:
	rm -f *.o $(EXE) 

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help