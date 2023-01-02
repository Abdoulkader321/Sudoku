EXE = sudoku

all: sudoku

sudoku:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) ./

report: ./report/main.tex ./report/LLP.bib
	pdflatex ./report/main.tex

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