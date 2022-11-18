#include "sudoku.h"

#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <err.h>
#include <getopt.h>

#include <grid.h>

typedef enum { mode_first, mode_all } mode_t;

static bool verbose = false; 
static size_t solutions;
static bool solved;


static grid_t *file_parser(char *filename)
{
  FILE *parsing_file = NULL;
  grid_t *grid = NULL;

  parsing_file = fopen(filename,"r");
  if(parsing_file == NULL)
  {
    warnx("error : can't open !file %s", filename);
    goto error;
  } 
  
  int current_char;

  // deciding when is starting the first row (not counting comments)
  do
  {
    current_char = fgetc(parsing_file);

    if(current_char == '#')
      while(current_char != '\n' && current_char != EOF) 
        current_char = fgetc(parsing_file);
    
    if(current_char == EOF)
    {
      warnx("error: EOF at beginning of the file %s", filename);
      goto error;
    }

  } while (current_char == '\t' || current_char == '\n' || 
    current_char == ' ' || current_char == '#');
  
  char first_row[MAX_GRID_SIZE];
  first_row[0] = current_char;
  size_t row_size = 1;
  current_char = fgetc(parsing_file);


  // fill first_row with first line characters
  while(current_char != EOF && current_char != '\n')
  {
    if(current_char != ' ' && current_char != '\t')
    {
      first_row[row_size] = current_char;
      row_size++;
    }
    current_char = fgetc(parsing_file);
  }

  if (!grid_check_size(row_size))
    {
      warnx("error: wrong line size in file %s",filename);
      goto error;
    }

  grid = grid_alloc(row_size);
  if(grid == NULL)
  {
    warnx("error: grid_alloc");
    goto error;
  }

  size_t grid_size = grid_get_size(grid);
  size_t line = 0;
  size_t column;

  // initializing first row of grid
  for(column = 0; column < grid_size; column++)
  {
    if(!grid_check_char(grid,first_row[column]))
    {
      warnx("error: wrong character '%c' at line 1!", first_row[column]);
      goto error;
    }
    grid_set_cell(grid,line,column,first_row[column]);
  }

  line++;
  column = 0;
  current_char = fgetc(parsing_file);
  
  // filling up the rest of the grid
  while(current_char != EOF)
  { 
    
    if(current_char == '#')
    { 
      bool comment = true;
      while(comment)
      {
        current_char = fgetc(parsing_file);
        if(current_char == '\n' || current_char == EOF) comment = false;
      }
    }

    else if(current_char == '\n')
    {
      if(column != 0)
      {
        if(column != grid_size)
        {
          warnx("error: wrong number of character at line %ld!",line +1);
          goto error;
        }

        column = 0;
        line++;
      }
    }
    
    else if(current_char != ' ' && current_char != '\t' && 
      current_char != EOF)
    {
      if(!grid_check_char(grid, current_char))
      {
        warnx("error: wrong character '%c' at line %ld!",current_char,line +1);
        goto error;
      }

      if(line >= grid_size)
      {
        warnx("error: grid has too many lines");
        goto error;
      }

      grid_set_cell(grid,line,column,current_char);
      column++;
    }

    current_char = fgetc(parsing_file);
  }

  if(column != 0)
  {
    if(column != grid_size)
    {
      warnx("error: line %ld is malformed! (wrong number of columns)",
    line +1);
    goto error;
    }
    column = 0;
    line++;
  }

  if(line != grid_size)
  {
    warnx("error: grid has wrong number of lines");
    goto error;
  }
  
  fclose(parsing_file);
  return grid;

  error: 

    if(grid != NULL) 
      grid_free(grid);

    if(parsing_file != NULL) 
      fclose(parsing_file);

    return NULL;
}
  
static void print_help()
{
  printf("Usage: sudoku [-a|-o FILE|-v|-V|-h] FILE...\n"
         "       sudoku -g[SIZE] [-u|-o FILE|-v|-V|-h]\n"
         "Solve or generate Sudoku grids of various sizes"
         " (1,4,9,16,25,36,49,64)\n\n"
         " -a,--all               search for all possible solutions\n"
         " -g[N],--generate[N]    generate a grid of size NxN (default:9)\n"
         " -u,--unique            generate a grid with unique solution\n"
         " -o FILE,--output FILE  write solution to FILE\n"
         " -v,--verbose           verbose output\n"
         " -V,--version           display version and exit\n"
         " -h,--help              display this help and exit\n");
}

static grid_t *grid_solver(grid_t *grid, FILE *fd, mode_t mode)
{
  if(grid == NULL)
    return NULL;
  
  size_t end_of_heuristics = grid_heuristics(grid);
  choice_t *choice;
  grid_t *copy;

  switch(end_of_heuristics)
  {
    case 2:
      grid_free(grid);
      return NULL;

    case 1: 
      if(mode == mode_all) 
      {
        solutions++;
        fprintf(fd,"solution #%ld:\n",solutions);
      }
      solved = true;
      grid_print(grid,fd);
      return grid;
    
    default:
      copy = grid_copy(grid);
      if(copy == NULL)
      {
        fprintf(fd,"erreur copy");
        grid_free(grid);
        return NULL;
      }

      choice = grid_choice(grid);
      if(choice == NULL)
      {
        fprintf(fd,"erreur choice");
        grid_free(grid);
        grid_free(copy);
        return NULL;
      }
      break;
  }

  grid_choice_apply(copy,choice);
  copy = grid_solver(copy,fd,mode);

  if(mode == mode_first)
  {
    if(copy)
    {
      grid_choice_free(choice);
      grid_free(grid);
      return copy;
    }

    grid_choice_discard(grid,choice);
    grid_free(copy);
    grid_choice_free(choice);
    return grid_solver(grid,fd,mode);
  }

  // mode_all ->
  grid_choice_discard(grid,choice);
  grid_free(copy);
  grid_choice_free(choice);
  return grid_solver(grid,fd,mode);
}

int main(int argc, char* argv[])
{

  const struct option long_opts[] = 
  {
    {"all",       no_argument,        NULL, 'a'},
    {"generate",  optional_argument,  NULL, 'g'},
    {"unique",    no_argument,        NULL, 'u'},
    {"output",    required_argument,  NULL, 'o'},
    {"verbose",   no_argument,        NULL, 'v'},
    {"version",   no_argument,        NULL, 'V'},
    {"help",      no_argument,        NULL, 'h'},
    {NULL,        0,                  NULL,  0 }
  };

  mode_t mode = mode_first;
  bool all = false;
  bool unique = false;
  bool generator = false;       // true = generator , false = solver
  bool inconsistency = false; 
  FILE* file = stdout;
  char* output_file = NULL;
  size_t size = DEFAULT_SIZE;


  int optc;

  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) != -1)
  switch (optc)
  {
  case 'a':
    if(generator)
    {
      warnx("warning: option all conflicts with generator mode, disabling "
        "it!");
      generator = false;
    }
    all = true;
    mode = mode_all;
    break;

  case 'g':
    generator = true;
    if(optarg != NULL)
    {
      int grid_size = strtol(optarg,NULL,10);
      if(!grid_check_size(grid_size))
         errx(EXIT_FAILURE,"error: you must enter size in (1, 4, 9, 16, 25," "36, 49, 64)");
      size = grid_size;
    }
    break;

  case 'u':
    if(!generator)
    {
      warnx("warning: option all conflicts with generator mode, disabling it!");
      generator = false;
    }
    unique = true;
    break;

  case 'o':
    if(optarg == NULL) 
      errx(EXIT_FAILURE,"error : no output file given");

    output_file = optarg;
    break;

  case 'v':
    verbose = true;
    if(argc < 3)
      errx(EXIT_FAILURE, "error : not enough options/arguments");
    break;
  
  case 'V':
    printf("sudoku %d.%d.%d\nSolve/generate sudoku grids (possible sizes:"
           " 1, 4, 9, 16, 25, 36, 49, 64)\n", VERSION, SUBVERSION, REVISION);
    exit(EXIT_SUCCESS);
    break;

  case 'h':
    print_help();
    exit(EXIT_SUCCESS);
    break;

  default:
    errx(EXIT_FAILURE, "error: invalid option '%s'!", argv[optind - 1]);
  }

  if(output_file)
  {
    file = fopen(output_file,"w");
    if(file == NULL)  
      errx(EXIT_FAILURE, "error : can't create file");  
  }

  if(!generator){
    if(optind == argc) errx(EXIT_FAILURE,"error : no grid given");

    for(int i = optind; i < argc; i++)   
    {                       
      FILE* checkfile = NULL;
      checkfile = fopen(argv[i], "r");
      if(checkfile == NULL) 
        errx(EXIT_FAILURE,"error : file not found");

      printf("file %s found and readable\n\n",argv[i]);
      fclose(checkfile);    
    }
  }
  
  
  if(!generator){
    
    inconsistency = false;
    if(optind == argc) errx(EXIT_FAILURE,"error : no grid given");

    for(int i = optind; i < argc; i++)   
    {                       
      grid_t *grid = file_parser(argv[i]);
      if(grid == NULL) errx(EXIT_FAILURE,"error: error with file %s", argv[i]);
      grid_print(grid,file);

      if(!grid_is_consistent(grid))
      {
        inconsistency = true;
        warnx("Grid %s is inconsistent !\n", argv[i]);
      }

      else 
      {
        solved = false;
        solutions = 0;
        grid = grid_solver(grid,file,mode);
        
        if(solved)
        {
          printf("The grid is solved!\n\n");
          if(mode == mode_all)
            fprintf(file,"number of solutions : %ld\n",solutions);
        }
        else
          printf("The grid hasn't been solved!\nThe grid is inconsistent\n\n");
        
        inconsistency |= !solved;
      }
      grid_free(grid);
    }
  }

  if(generator)
  {
    grid_t *grid = grid_alloc(size);
    grid_print(grid,file);
    grid_free(grid);
  }

  if(file != stdout) 
  {
    fclose(file);
  }

  if(!generator && inconsistency)
    errx(EXIT_FAILURE,"error: one of the given grids is inconsistent");

  return EXIT_SUCCESS;
}



