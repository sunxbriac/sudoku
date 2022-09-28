#include "sudoku.h"

int main(int argc, char* argv[])
{

  static struct option long_opts[] = 
  {
    {"all",       no_argument,        0, 'a'},
    {"generate",  optional_argument,  0, 'g'},
    {"unique",    no_argument,        0, 'u'},
    {"output",    required_argument,  0, 'o'},
    {"verbose",   no_argument,        0, 'v'},
    {"version",   no_argument,        0, 'V'},
    {"help",      no_argument,        0, 'h'},
    {0,           0,                  0,  0 }
  };
  
  static bool verbose = false; 
  static bool all = false;
  static bool unique = false;
  static bool generator = true;       // true = generator , false = solver 
  FILE* file = NULL;

  int optc;

  while ((optc = getopt_long(argc, argv, "ag::uo:vVh", long_opts, NULL)) != -1)
  switch (optc)
  {
  case 'a':
    if(generator)
    {
      warnx("warning: option all conflicts with generator mode, disabling it!");
      generator = false;
    }
    all = true;
    break;

  case 'g':
    if(!generator)
    {
      warnx("warning: option generate conflicts with solver mode, "
            "disabling it!");
      generator = true;
    }
    if(optarg != NULL)
    {
      int taille = strtol(optarg,NULL,10);
      if(taille != 1 && taille != 4 && taille != 9 && taille != 16 &&
        taille != 25 && taille != 36 && taille != 49 && taille != 64)
        errx(EXIT_FAILURE,"error: grid size not handled by this generator");
      // insérer fontion pour générer la grille
      exit(EXIT_SUCCESS);
    }
    exit(EXIT_SUCCESS);
    break;

  case 'u':
    unique = true;
    break;

  case 'o':
    file = fopen(optarg,"r+");
    if(file == NULL)
      errx(EXIT_FAILURE, "error : file not found in current repertory");  
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

  int i = 0;

  if(verbose)   // un argument en plus si il y a l'option verbose
    i = 2;
  else
  {
    if(argc < 2) 
      errx(EXIT_FAILURE, "error : not enough options/arguments");
    i = 1;
  }

  for(i; i < argc; i++)   // boucle pour regarder si les fichiers sont présents
  {                       // et plus tard vérifier si ils sont de bon format...
    FILE* file = NULL;
    file = fopen(argv[i], "r+");
    if(file == NULL) 
      errx(EXIT_FAILURE,"error : file not found");
    fclose(file);
  }
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

