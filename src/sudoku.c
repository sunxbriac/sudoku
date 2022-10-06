#include "sudoku.h"

static bool verbose = false; 

int main(int argc, char* argv[])
{

  static struct option long_opts[] = 
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
      if(grid_size != 1 && grid_size != 4 && grid_size != 9 && grid_size != 16 
        && grid_size != 25 && grid_size != 36 && grid_size != 49 && 
        grid_size != 64) errx(EXIT_FAILURE,"error: you must enter size in "
        "(1, 4, 9, 16, 25, 36, 49, 64)");
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

  for(int j = 0; j < argc; j++)
  { 
    printf("j = %d\n", j);
    printf("%s\n",argv[j]);
  }

  if(!generator){
    if(optind == argc) errx(EXIT_FAILURE,"error : no grid given");

    for(int i = optind; i < argc; i++)   
    {                       
      FILE* checkfile = NULL;
      checkfile = fopen(argv[i], "r");
      if(checkfile == NULL) 
        errx(EXIT_FAILURE,"error : file not found");
      else printf("file %s found and readable\n",argv[i]);
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

void print_help()
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

static grid_t *grid_alloc (size_t size)
{

  char **cells = NULL;
  cells = malloc(size *sizeof(char*));
  if(cells == NULL) errx(EXIT_FAILURE,"error : malloc...");

  for(size_t i = 0; i < size; i++)
  {
    char *cell = NULL;
    cell = malloc(size *sizeof(char));
    if(cell == NULL) errx(EXIT_FAILURE,"error : malloc...");
    for(size_t j = 0; j < size; j++)
    {
      cell[j] = DEFAULT_CELL;
    }
    cells[i] = cell;
  }

  grid_t *grid;
  grid = malloc(sizeof(grid_t));
  if(grid == NULL) errx(EXIT_FAILURE,"error : malloc...");
  grid->size = size;
  grid->cells = cells;
  return grid;
}

static void grid_print(const grid_t *grid, FILE *fd)
{
  for(size_t i = 0; i  < grid->size; i++)
  {
    for(size_t j = 0; j + 1 < grid->size ; j++)
      fprintf(fd, "%c ", grid->cells[i][j]);
    fprintf(fd, "%c\n", grid->cells[i][grid->size - 1]);
  }
}

static void grid_free(grid_t *grid)
{
  for(size_t i = 0; i < grid->size; i++)
    free(grid->cells[i]);
  free(grid->cells);
  free(grid);
}

static bool check_char(const grid_t *grid, const char c)
{
  switch(grid->size)
  {
    case 1:
      if(c == DEFAULT_CELL || c == '1') return true;
      return false;
    case 4:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '4')) return true;
      return false;
    case 9:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9')) return true;
      return false;
    case 16:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9') || 
        (c >= 'A' &&  c <= 'G')) return true;
      return false;
    case 25:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9') || (c >= 'A' && c <= 'P'))
        return true;
      return false;
    case 36:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9') || (c >= '@' && c <= 'Z'))
        return true;
      return false;
    case 49:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z') 
       || (c >= 'a' && c <= 'm')) return true;
      return false;
    case 64:
      if(c == DEFAULT_CELL || (c >= '1' && c <= '9') || (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z') || (c == '&' || c == '*')) return true;
      return false;
    default:
      return false;
  }
}

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
  int row_size = 1;
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

  if(row_size != 1 && row_size != 4 && row_size != 9 && row_size != 16 && 
    row_size != 25 && row_size != 36 && row_size != 49 && row_size != 64)
    {
    warnx("error: wrong line size in file %s",filename);
    goto error;
    }

  grid = grid_alloc((size_t)row_size);
  if(grid == NULL)
  {
    warnx("error: grid_alloc");
    goto error;
  }
 
  size_t line = 0;
  size_t column = 0;

  // initializing first row of grid
  for(; column < grid->size; column++)
  {
    if(check_char(grid,first_row[column]))
      grid->cells[line][column] = first_row[column];
    else {
      warnx("error: wrong character '%c' at line 1!", first_row[column]);
      goto error;
    }
  }

  line++;
  
  // filling up the rest of the grid
  
  while(current_char != EOF && line < grid->size)
  { // on prend le premier caracatère de la ligne et on regarde ce qu'il vaut
    current_char = fgetc(parsing_file);
    // on passe à la prochaine itération si on saute une ligne
    while(current_char == '\n')
      current_char = fgetc(parsing_file);
    
    // on passe la ligne entière si on a un commentaire
    if(current_char == '#')
    { 
      bool comment = true;
      while(comment)
      {
        current_char = fgetc(parsing_file);
        if(current_char == '\n' || current_char == EOF) comment = false;
      }
    }
    // si ligne commence par tab ou espace = ligne vide qu'on ignore
    else if(current_char == '\t' || current_char == ' ')
    {
      while(current_char != '\n' && current_char != EOF)  
        current_char = fgetc(parsing_file);
    }
    
    else if(!(check_char(grid,current_char)))
    {
      warnx("error: wrong character '%c' at line %ld!",current_char,line +1);
      goto error;
    }
  
    else
    {
      column = 0;
      grid->cells[line][column] = current_char;
      column++;
      // fill the line of index line
      while(column < grid->size && current_char != EOF)
      {
        current_char = fgetc(parsing_file);
        // on regarde si c'est un caractère 
        if(check_char(grid,current_char))
        {
          grid->cells[line][column] = current_char;
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
              if(column < grid->size)
              {
                warnx("error: line %ld is malformed! (wrong number of columns!"
                ,line +1);
                goto error;
              } // revenir pcq on sait jamais ça peut ne pas compiler sans break

            case EOF:
              break;

            default:
              warnx("error: wrong character '%c' at line %ld!",
               current_char, line +1);
              goto error;
          }
        }
        line++;
        // on a regardé le caractère et fait rien si blablabla
      } // on sort de la boucle pour remplir la ligne
      
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
          if(line < grid->size)
          {
            warnx("error: grid has %ld missing line(s)!", grid->size -line);
            goto error;
          }
          break;

        default:
          warnx("error: line %ld is malformed! (wrong number of columns)",
           line);
          goto error;
        } 
      } // fin de vérification fin de ligne
    
    } // fin du else si le  premier caractère est viable
    
  } // fin du while pour remplir la grille

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
      
      case '\0':
        end = true;

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
  



