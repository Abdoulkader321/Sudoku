EXE = sudoku

all: sudoku

%:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) ./
	
clean:
	@cd src && $(MAKE) clean
	@rm -f $(EXE) *.txt

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help