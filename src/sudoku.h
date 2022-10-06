#ifndef SUDOKU_H
#define SUDOKU_H

#define VERSION       1
#define SUBVERSION    0
#define REVISION      0
#define DEFAULT_CELL  '_'
#define DEFAULT_SIZE  9
#define MAX_GRID_SIZE 64

#endif /* SUDOKU_H */

#include <err.h>
#include <getopt.h>
#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct 
{
  size_t size;
  char **cells;
} grid_t;

void print_help();
static grid_t *grid_alloc (size_t size);
static void grid_print(const grid_t *grid, FILE *fd);
static void grid_free(grid_t *grid);
static bool check_char(const grid_t *grid, const char c);
static grid_t *file_parser(char *filename);