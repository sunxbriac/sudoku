CFLAGS = -std=c11 -Wall -Wextra -g -pedantic
CPPFLAGS = -I../include -DEBUG
LDFLAGS =
EXE = sudoku

all : sudoku 

test : colors_tests grid_tests

sudoku : sudoku.o grid.o colors.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ 

sudoku.o : sudoku.c sudoku.h ../include/colors.h ../include/grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

colors.o : colors.c ../include/colors.h ../include/grid.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

grid.o : grid.c ../include/grid.h ../include/colors.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

colors_tests : colors_tests.o colors.o
	gcc -o colors_tests colors_tests.o colors.o

colors_tests.o : colors_tests.c
	gcc -I ../include -c colors_tests.c 
	
grid_tests : grid_tests.o grid.o
	gcc -o grid_tests grid_tests.o grid.o colors.o

grid_tests.o : grid_tests.c
	gcc -I ../include -c grid_tests.c

clean : 
	@rm -rf *.o $(EXE)

help : 
	echo "Usage : "
	@echo "  make [all]\t\tBuild the software"
	@echo "  make clean\t\tRemove all files"
	@echo "  make help\t\tDisplay this help"
	
.PHONY : all clean help