#include "sudoku.h"

#include <err.h>
#include <getopt.h>
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



static bool verbose = false; 



static grid_t *file_parser(char *filename)
{
  FILE *parsing_file = NULL;
  grid_t *grid = NULL;
  parsing_file = fopen(filename,"r");
  if(parsing_file == NULL)
  {
    warnx("error : can't open file %s", filename);
    goto error;
  } 
  
  char current_char;

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

  } while (current_char == '\t' || current_char == '\n' || current_char == ' ' 
  || current_char == '#');
  
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
  size_t column = 0;

  // initializing first row of grid
  for(; column < grid_size; column++)
  {
    if(grid_check_char(grid,first_row[column]))
      grid_set_cell(grid,line,column,first_row[column]);
    else {
      warnx("error: wrong character '%c' at line 1!", first_row[column]);
      goto error;
    }
  }

  line++;
  
  // filling up the rest of the grid
  while(current_char != EOF && line < grid_size)
  { 
    current_char = fgetc(parsing_file);
   
    while(current_char == '\n')
      current_char = fgetc(parsing_file);
    
    if(current_char == EOF)
    {
      warnx("error: grid has %ld missing line(s)!", grid_size -line);
      goto error;
    }
    
    if(current_char == '#')
    { 
      bool comment = true;
      while(comment)
      {
        current_char = fgetc(parsing_file);
        if(current_char == '\n' || current_char == EOF) comment = false;
      }
    }

    else if(current_char == '\t' || current_char == ' ')
    {
      while(current_char != '\n' && current_char != EOF)  
        current_char = fgetc(parsing_file);
    }
    
    else if(!(grid_check_char(grid,current_char)))
    {
      warnx("error: wrong character '%c' at line %ld!",current_char,line +1);
      goto error;
    }
    

    else
    {
      column = 0;
      grid_set_cell(grid,line,column,current_char);
      column++;

      while(column < grid_size && current_char != EOF)
      {
        current_char = fgetc(parsing_file);

        if(grid_check_char(grid,current_char))
        {
          grid_set_cell(grid,line,column,current_char);
          column++;
        }
        else 
        {
          switch (current_char)
          {
            case ' ':
              break;
        
            case '\t':
              break;
        
            case '\n':
              if(column < grid_size)
              {
                warnx("error: line %ld is malformed! (wrong number of columns)"
                ,line +1);
                goto error;
              } 

            case EOF:
              break;

            default:
              warnx("error: wrong character '%c' at line %ld!",
               current_char, line +1);
              goto error;
          }
        }

      } 
      line++;

      while(current_char != '\n' && current_char != '\0')
      {
        current_char = fgetc(parsing_file);
        switch (current_char)
        {
        case ' ':
          break;
        
        case '\t':
          break;
        
        case '\n':
          break;
        
        case '\0':
          break;

        case EOF:
          if(line < grid_size)
          {
            warnx("error: grid has %ld missing line(s)!", grid_size -line);
            goto error;
          }
          if(column < grid_size)
          {
            warnx("error: grid has %ld missing character(s) at line %ld",
             grid_size - column, line );
            goto error;
          }

          fclose(parsing_file);
          return grid;
          break;

        default:
          warnx("error: line %ld is malformed! (wrong number of columns)",
           line);
          goto error;
        } 
      } 
    } 
  } 

// grid is filled, exit with success if there are no other sudoku lines in file
  if(current_char == EOF || current_char == '\0')
  {
    fclose(parsing_file);
    return grid;
  }
  else 
  {
    bool end = false;
    while(!end)
    {
      switch (current_char)
      {
      case '#':
        while(current_char != '\n'){
          current_char = fgetc(parsing_file);
          if(current_char == EOF) end = true;
        }
        break;

      case '\n':
        while(current_char == '\n') current_char = fgetc(parsing_file);
        break;

      case ' ':
        while(current_char == ' ' || current_char == '\t')
          current_char = fgetc(parsing_file);
        break;
      
      case '\t':
        while(current_char == ' ' || current_char == '\t')
          current_char = fgetc(parsing_file);
        break;
      
      case EOF:
        end = true;
        break;
      
      case '\0':
        end = true;
        break;

      default:
        warnx("error: grid has too many lines");
        goto error;
      }
    }
    fclose(parsing_file);
    return grid;
  }

  error: 
    if(grid != NULL) grid_free(grid);
    if(parsing_file != NULL) fclose(parsing_file);

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

  bool all = false;
  bool unique = false;
  bool generator = false;       // true = generator , false = solver 
  FILE* file = stdout;
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
    if(optarg == NULL) errx(EXIT_FAILURE,"error : no output file given");

    file = fopen(optarg,"w");
    if(file == NULL)    // no such file in directory
      errx(EXIT_FAILURE, "error : can't create file");  
    fclose(file);
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

  if(!generator){
    if(optind == argc) errx(EXIT_FAILURE,"error : no grid given");

    for(int i = optind; i < argc; i++)   
    {                       
      FILE* checkfile = NULL;
      checkfile = fopen(argv[i], "r");
      if(checkfile == NULL) 
        errx(EXIT_FAILURE,"error : file not found");

      printf("file %s found and readable\n",argv[i]);
      fclose(checkfile);    // plus tard on rentre dedans
    }
  }
  
  
  if(!generator){
    if(optind == argc) errx(EXIT_FAILURE,"error : no grid given");

    for(int i = optind; i < argc; i++)   
    {                       
      grid_t *grid = file_parser(argv[i]);
      if(grid == NULL) errx(EXIT_FAILURE,"error: error with file %s", argv[i]);
      grid_print(grid,file);
      grid_free(grid);
    }
  }
  
  if(generator)
  {
    grid_t *grid = grid_alloc(size);
    grid_print(grid,file);
    grid_free(grid);
  }

  if(file != NULL) fclose(file);
  return 0;
 
}



