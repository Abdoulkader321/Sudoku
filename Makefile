EXE = sudoku

all: sudoku

sudoku:
	@cd src && $(MAKE)
	@cp -f src/$(EXE) ./

report: ./report/report.tex ./report/LLP.bib
	-@cd report; pdflatex -interaction=nonstopmode report.tex; biber report; pdflatex -interaction=nonstopmode report.tex; 
	@cd report; rm *.bbl *.aux *.blg *.log *.bcf *.run.xml *.out
	@rm *.log

clean:
	@cd src && $(MAKE) clean
	@rm -f $(EXE) *.txt
	@cd tests && $(MAKE) clean

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild tests"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all report clean help