CFLAGS = -std=c11 -Wall -Wextra -g -pedantic

all : sudoku

sudoku : sudoku.o
	$(CC) -o $@ $^ $(CFLAGS)

sudoku.o : sudoku.c sudoku.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean : 
	@rm -rf *.o

help : 
	echo "Usage : "
	@echo "  make [all]\t\tBuild the software"
	@echo "  make clean\t\tRemove all files"
	@echo "  make help\t\tDisplay this help"
	