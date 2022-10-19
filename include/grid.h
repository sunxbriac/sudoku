#include <stdbool.h>
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>

#ifndef GRID_H
#define GRID_H

#define MAX_GRID_SIZE 64
#define EMPTY_CELL    '_'

static const char color_table[] = 
  "123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "@" "abcdefghijklmnopqrstuvwxyz" "&*";

typedef struct _grid_t grid_t;

#endif /* GRID_H */

bool grid_check_char(const grid_t *grid, const char c);
/* checks if a character fits to a grid of this grid's size */

grid_t *grid_alloc (size_t size);
/* allocates memory and creates a grid of size size */

void grid_free(grid_t *grid);
/* releases the memory allocated for this grid */

void grid_print(const grid_t *grid, FILE *fd);
/* displays the grid in a sudoku format */

bool grid_check_size (const size_t size);
/* checks if the grid size belongs to {1;4;9;16;25;36;49;64} */

grid_t *grid_copy (const grid_t *grid);
/* returns a copy of the given grid */

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column);
/* gets the string of colors in the cell of the given grid */

size_t grid_get_size(const grid_t *grid);
/* returns the size of the current grid */

void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
 const char color);
/* set the content of the grid's cell to a specific value */
