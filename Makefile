EXE = sudoku

all : 
	@cd src && $(MAKE)
	@mv src/sudoku sudoku

clean : 
	@cd src && $(MAKE) clean
	@rm -f $(EXE)

help : 
	@echo "Usage : "
	@echo "  make [all]\t\tCall source Make to build the software"
	@echo "  make clean\t\tRemove all files and outdated software"
	@echo "  make help\t\tDisplay this help"
