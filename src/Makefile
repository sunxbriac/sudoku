CFLAGS = -std=c11 -Wall -Wextra -pedantic -O3
CPPFLAGS = -I../include -DEBUG
LDFLAGS =
EXE = sudoku

all : sudoku 

test : colors_tests grid_tests

sudoku : sudoku.o grid.o colors.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o  $@ $^ 

sudoku.o : sudoku.c sudoku.h ../include/grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

colors.o : colors.c ../include/colors.h 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

grid.o : grid.c ../include/grid.h ../include/colors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

clean : 
	@rm -rf *.o $(EXE)

help : 
	echo "Usage : "
	@echo "  make [all]\t\tBuild the software"
	@echo "  make clean\t\tRemove all files"
	@echo "  make help\t\tDisplay this help"
	
.PHONY : all clean help