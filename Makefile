EXE = sudoku

all: sudoku

%:
	cd src; make; cd ..;

clean:
	cd src; make clean; cd ..;

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help