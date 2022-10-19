EXE = sudoku

all: sudoku

sudoku:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) ./
	
clean:
	@cd src && $(MAKE) clean
	@rm -f $(EXE) *.txt
	@cd tests && $(MAKE) clean

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild tests"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help