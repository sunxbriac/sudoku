#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>

#define MAX_GRID_SIZE 64
#define EMPTY_CELL    '_'

static const char color_table[] = 
  "123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "@" "abcdefghijklmnopqrstuvwxyz" "&*";

typedef struct _grid_t grid_t;

/* Sudoku grid choice (forward declaration to hide the implementation) */
typedef struct choice_t choice_t;

/* checks if a character fits to a grid of this grid's size */
bool grid_check_char(const grid_t *grid, const char c);

/* allocates memory and creates a grid of size size */
grid_t *grid_alloc(size_t size);

/* releases the memory allocated for this grid */
void grid_free(grid_t *grid);

/* displays the grid in a sudoku format */
void grid_print(const grid_t *grid, FILE *fd);

/* checks if the grid size belongs to {1;4;9;16;25;36;49;64} */
bool grid_check_size(const size_t size);

/* returns a copy of the given grid */
grid_t *grid_copy(const grid_t *grid);

/* gets the string of colors in the cell of the given grid */
char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column);

/* gets the string of colors in the choice */
char *choice_get_cell(const choice_t *choice);

/* returns the size of the current grid */
size_t grid_get_size(const grid_t *grid);

/* set the content of the grid's cell to a specific value */
void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
 const char color);

/* checks if a grid is solved */
bool grid_is_solved(grid_t *grid);

/* checks if a grid is consistent */
bool grid_is_consistent(grid_t *grid);

/* applies heuritstics to a given grid */
size_t grid_heuristics(grid_t *grid);

/* frees a choice_t */
void grid_choice_free(choice_t *choice);

/* checks if the color set of the choice is empty */
bool grid_choice_is_empty(const choice_t *choice);

/* applies the choice to the given grid */
void grid_choice_apply(grid_t *grid, const choice_t *choice);

/* sets to full color the given choice */
void grid_choice_blank(grid_t *grid, const choice_t *choice);

/* discards the choice from the grid */
void grid_choice_discard(grid_t *grid, const choice_t *choice);

/* write the choice on the file descriptor */
void grid_choice_print(const choice_t *choice, FILE *fd);

/* chooses the smallest set of colors in all the grid 
   and select the rightmost color */
choice_t *grid_choice(grid_t *grid);

/* sets every color of the grid to empty */
void grid_set_empty(grid_t *grid);

#endif /* GRID_H */