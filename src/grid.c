#include "grid.h"
#include <colors.h>

#include <err.h>
#include <stdio.h>


/* internal structure (hidden from outside) to represent a sudoku grid */
struct _grid_t 
{
  size_t size;
  colors_t **cells;
};

bool grid_check_char(const grid_t *grid, const char c)
{
  if(grid == NULL) return false;
  if(c == '_') return true;

  if(c > 'z') return false;
  if(c > 'm' || c == '*' || c == '&') return (grid->size == 64);
  if(c >= 'a') return (grid->size >= 49);
  if(c > 'Z') return false;
  if(c > 'P' || c == '@') return (grid->size >= 36);
  if(c > 'G') return (grid->size >= 25);
  if(c >= 'A') return (grid-> size >= 16);
  if(c > '9') return false;
  if(c > '4') return (grid->size >= 9);
  if(c > '1') return (grid->size >= 4);
  if(c == '1') return (grid->size >= 1);

  return false;
}

grid_t *grid_alloc (size_t size)
{
  if (!grid_check_size(size)) return NULL;

  colors_t **cells = NULL;
  cells = malloc (size * sizeof(colors_t*));
  if (cells == NULL) return NULL;

  for(size_t i = 0; i < size; i++)
  {
    colors_t *cell = NULL;
    cell = malloc (size * sizeof(colors_t));
    if (cell == NULL) return NULL;

    for (size_t j = 0; j < size; j++)
    {
      cell[j] = colors_empty();
    }
    cells[i] = cell;
  }

  grid_t *grid;
  grid = malloc (sizeof(grid_t));
  if (grid == NULL) return NULL;

  grid->size = size;
  grid->cells = cells;
  return grid;
}

void grid_free(grid_t *grid)
{
  if (grid == NULL) return;

  for (size_t i = 0; i < grid->size; i++)
    free (grid->cells[i]);
  free (grid->cells);
  free (grid);
}

void grid_print(const grid_t *grid, FILE *fd)
{
  if(grid == NULL) return;

  for(size_t i = 0; i  < grid->size; i++)
  {
    for(size_t j = 0; j + 1 < grid->size ; j++)
      fprintf(fd, "%s ", grid_get_cell(grid,i,j));
    fprintf(fd, "%s\n", grid_get_cell(grid,i,grid->size -1));
  }
}

bool grid_check_size (const size_t size)
{
  return (size == 1 || size == 4 || size == 9 || size == 16 || size == 25
          || size == 36 || size == 49 || size == 64);
}

grid_t *grid_copy (const grid_t *grid)
{
  if (grid == NULL) return NULL;

  grid_t *grid_copy = grid_alloc(grid->size);
  if (grid_copy == NULL) return NULL;

  for(size_t i = 0; i < grid->size; i++)
  {
    for (size_t j = 0; j < grid->size; j++)
    {
      grid_copy->cells[i][j] = grid->cells[i][j];
    }
  }

  return grid_copy;
}

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column)
{
  if(grid == 0 || row >= grid-> size || column >= grid->size) return NULL;

  colors_t cell = grid->cells[row][column];
  size_t count = colors_count(cell);

  char *cell_str = malloc ((count + 1) * sizeof(char));
  if(cell_str == NULL) 
  {
    warnx("Error : malloc in grid_get_cell.");
    return NULL;
  }

  size_t i = 0;
  size_t color_id = 0;
  while (i < count )
  {
    if (colors_is_in(cell,color_id))
    {
      cell_str[i] = color_table[color_id];
      i++;
    }
    color_id++;
  }

  cell_str[i] = '\0';
  return cell_str;
}

size_t grid_get_size(const grid_t *grid)
{
  return (grid == NULL) ? 0 : grid->size;
}

void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
 const char color)
{
  if(grid == 0 || row >= grid-> size || column >= grid->size) return;
  
  size_t i = 0;
  while (color != color_table[i] && (i - 1) < grid->size) i++;

  grid->cells[row][column] = colors_set(i);
}

