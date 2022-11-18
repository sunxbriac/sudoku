#include "grid.h"

#include <stdio.h>

#include <colors.h>


/* internal structure (hidden from outside) to represent a sudoku grid */
struct _grid_t 
{
  size_t size;
  colors_t **cells;
};

struct choice_t 
{
  size_t row;
  size_t column;
  colors_t color;
};


static size_t square_root(size_t size)
{
  switch (size)
  {
  case 1:
    return 1;
  case 4:
    return 2;
  case 9:
    return 3;
  case 16:
    return 4;
  case 25:
    return 5;
  case 36:
    return 6;
  case 49:
    return 7;
  case 64:
    return 8;

  default:
    return 0;
  }
}

bool grid_check_char(const grid_t *grid, const char c)
{
  if(grid == NULL) return false;
  if(c == EMPTY_CELL) return true;

  for(size_t i = 0; i < grid->size; i++)
    if(c == color_table[i]) 
      return true;

  return false;
}

grid_t *grid_alloc(size_t size)
{
  if(!grid_check_size(size)) return NULL;

  colors_t **cells = NULL;
  cells = malloc(size * sizeof(colors_t*));
  if(cells == NULL) return NULL;

  for(size_t i = 0; i < size; i++)
  {
    colors_t *cell = NULL;
    cell = malloc(size * sizeof(colors_t));
    if(cell == NULL) return NULL;

    for(size_t j = 0; j < size; j++)
    {
      cell[j] = colors_empty();
    }
    cells[i] = cell;
  }

  grid_t *grid;
  grid = malloc(sizeof(grid_t));
  if(grid == NULL) return NULL;

  grid->size = size;
  grid->cells = cells;
  return grid;
}

void grid_free(grid_t *grid)
{
  if(grid == NULL) return;

  for(size_t i = 0; i < grid->size; i++)
    free(grid->cells[i]);
  free(grid->cells);
  free(grid);
}

void grid_print(const grid_t *grid, FILE *fd)
{
  if(grid == NULL) return;

/* We search for the biggest string to print in the grid to make it 
 * more readable when we are using big sized grids */

  char *c_grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
  size_t max = 0;

  for(size_t i = 0; i  < grid->size; i++)
  {
    for(size_t j = 0; j < grid->size ; j++)
    {
      char *c = grid_get_cell(grid,i,j);
      if(c == NULL) 
        return;

      c_grid[i][j] = c;

      size_t m = 0;
      while((m < grid->size) && (c[m] != '\0')) m++;
      if(m < grid->size) max = (m > max)? m : max;
    }
  }

  
  for(size_t i = 0; i  < grid->size; i++)
  {
    for(size_t j = 0; j < grid->size ; j++)
    {
      size_t n = 0;
      while((n < grid->size) && (c_grid[i][j][n] != '\0')) n++;

      if(n == grid->size)
      {
        fprintf(fd, "%c ", (n == 1)?'1':EMPTY_CELL);
      }
      else
        fprintf(fd, "%s ", c_grid[i][j]); 
      
      for(size_t k = 0; k+n < max; k++)
        fprintf(fd," ");
      
      free(c_grid[i][j]);
    }
    fprintf(fd, "\n");
    
  }

  fprintf(fd,"\n");
}

bool grid_check_size(const size_t size)
{
  return(size == 1 || size == 4 || size == 9 || size == 16 || size == 25
          || size == 36 || size == 49 || size == 64);
}

grid_t *grid_copy(const grid_t *grid)
{
  if(grid == NULL) return NULL;

  grid_t *grid_copy = grid_alloc(grid->size);
  if(grid_copy == NULL) return NULL;

  for(size_t i = 0; i < grid->size; i++)
  {
    for(size_t j = 0; j < grid->size; j++)
    {
      grid_copy->cells[i][j] = grid->cells[i][j];
    }
  }

  return grid_copy;
}

static char *colors_to_string(const colors_t colors)
{
  size_t count = colors_count(colors);

  char *cell_str = malloc ((count + 1) * sizeof(char));
  if(cell_str == NULL) 
    return NULL;
  
  size_t i = 0;
  size_t color_id = 0;
  while (i < count)
  {
    if(colors_is_in(colors,color_id))
    {
      cell_str[i] = color_table[color_id];
      i++;
    }
    color_id++;
  }
  
  cell_str[i] = '\0';
  return cell_str;
}

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column)
{
  if(grid == 0 || row >= grid-> size || column >= grid->size) return NULL;

  colors_t cell = grid->cells[row][column];
  return colors_to_string(cell);
}

size_t grid_get_size(const grid_t *grid)
{
  return(grid == NULL) ? 0 : grid->size;
}

void grid_set_cell(grid_t *grid, const size_t row, const size_t column,
 const char color)
{
  if(grid == NULL || row >= grid-> size || column >= grid->size) return;
  
  size_t i = 0;
  while((color != color_table[i]) && (i < grid->size)) i++;

  if(i < grid->size)
    grid->cells[row][column] = colors_set(i);
  else 
    grid->cells[row][column] = colors_full(i);
}


bool grid_is_solved(grid_t *grid)
{
  for(size_t i = 0; i < grid->size; i++)
    for(size_t j = 0; j < grid->size; j++)
      if(!colors_is_singleton(grid->cells[i][j])) return false;
  return true;
}

bool grid_is_consistent(grid_t *grid)
{
  
  // lines and column subgrids 

  colors_t lines[MAX_GRID_SIZE][MAX_GRID_SIZE];
  colors_t columns[MAX_GRID_SIZE][MAX_GRID_SIZE];
  
  for(size_t i = 0; i < grid->size ; i++)
  {
    for(size_t j = 0; j < grid->size; j++)
    {
      lines[i][j] = grid->cells[i][j];
      columns[i][j] = grid->cells[j][i];
    }
  }
  
  // blocks subgrids 

  size_t sqrt = square_root(grid->size);
  colors_t blocks[MAX_GRID_SIZE][MAX_GRID_SIZE];

  int n = 0;
  for(size_t i = 0; i < sqrt; i++ )
  {
    for(size_t j = 0; j < sqrt; j++)
    {
      int m = 0;

      for(size_t k = 0; k < sqrt; k++)
        for(size_t l = 0; l < sqrt; l++)
          blocks[n][m++] = grid->cells[i*sqrt + k][j*sqrt + l];
      
      n++;     
    }
  }


  for(size_t i = 0; i < grid->size; i++)
  {
    if(!subgrid_consistency(lines[i],grid->size) ||
        !subgrid_consistency(columns[i],grid->size) ||
          !subgrid_consistency(blocks[i],grid->size))
      return false;
  }

  return true;
}

size_t grid_heuristics(grid_t *grid)
{
  bool keep_going = true;
// lines and column subgrids 

  colors_t *lines[MAX_GRID_SIZE][MAX_GRID_SIZE];
  colors_t *columns[MAX_GRID_SIZE][MAX_GRID_SIZE];

  for(size_t i = 0; i < grid->size; i++)
    {
      for(size_t j = 0; j < grid->size; j++)
      {
        lines[i][j] = &grid->cells[i][j];
        columns[i][j] = &grid->cells[j][i];
      }
    }

  // blocks subgrids 

  size_t sqrt = square_root(grid->size);
  colors_t *blocks[MAX_GRID_SIZE][MAX_GRID_SIZE];

  size_t n = 0;
  
  for(size_t i = 0; i < sqrt; i++ )
  {
    for(size_t j = 0; j < sqrt; j++)
    {
      size_t m = 0;

      for(size_t k = 0; k < (size_t)sqrt; k++)
        for(size_t l = 0; l < (size_t)sqrt; l++)
          blocks[n][m++] = &grid->cells[i*sqrt +k][ j*sqrt+l];

      n++;
    }
  }

  while(keep_going)
  {
    keep_going = false;
    for(size_t i = 0; i < grid->size; i++)
    {
      bool keep_tmp = subgrid_heuristics(lines[i],grid->size) |
      subgrid_heuristics(columns[i],grid->size) |
      subgrid_heuristics(blocks[i],grid->size);
      keep_going |= keep_tmp;
    }
    
    if(!grid_is_consistent(grid) || grid_is_solved(grid)) keep_going = false;
  }


  return (grid_is_consistent(grid))? ((grid_is_solved(grid))? 1 : 0) : 2;
}

void grid_choice_free(choice_t *choice)
{
  if(choice == NULL) return;
  free(choice);
}

bool grid_choice_is_empty(const choice_t *choice)
{
  return (choice->color == colors_empty());
}

void grid_choice_apply(grid_t *grid, const choice_t *choice)
{
  grid->cells[choice->row][choice->column] = choice->color;
}

void grid_choice_blank(grid_t *grid, const choice_t *choice)
{
  grid->cells[choice->row][choice->column] = colors_full(grid->size);
}

void grid_choice_discard(grid_t *grid, const choice_t *choice)
{
  grid->cells[choice->row][choice->column] &= ~choice->color;
}

void grid_choice_print(const choice_t *choice, FILE *fd)
{
  char *c = colors_to_string(choice->color);
  if(c == NULL) 
    return;
  
  fprintf(fd, "Next choice at grid[%ld][%ld] ",choice->row,choice->column);
  fprintf(fd, "is '%s'.\n",c);

  free(c);
}

choice_t *grid_choice(grid_t *grid) 
{
  size_t min = grid->size;
  size_t min_row;
  size_t min_col;

  for(size_t i = 0; i < grid->size; i++)
  {
    for(size_t j = 0; j < grid->size; j++)
    {
      size_t count = colors_count(grid->cells[i][j]);
      if((count < min) && (count > 1))
      {
        min = count;
        min_row = i;
        min_col = j;
      }
    }
  }

  if(min == grid->size) 
    return NULL;

  choice_t *choice = malloc(sizeof(choice_t));
  choice->row = min_row;
  choice->column = min_col;
  choice->color = colors_rightmost(grid->cells[min_row][min_col]);

  return choice;
}
