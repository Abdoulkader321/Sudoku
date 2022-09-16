CFLAGS = -std=c11 -Wall -Wextra -g -O2
CPPFLAGS = -I../include -DDEBUG
LDFLAGS = -lm

EXE = sudoku


all: sudoku

%: %.c %.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS);

clean:
	rm -f *.o $(EXE) 

help:
	@echo "USAGE:"
	@echo "  make\t\t\tBuild sudoku"
	@echo "  make clean\t\tRemove all files produced by the compilation"
	@echo "  make help\t\tDisplay this help"

.PHONY: all clean help