#include "grid.h"

#include <stdio.h>

#include <colors.h>


/* internal structure (hidden from outside) to represent a sudoku grid */
struct _grid_t 
{
  size_t size;
  colors_t **cells;
};

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

  for(size_t i = 0; i  < grid->size; i++)
  {
    for(size_t j = 0; j < grid->size ; j++)
    {
      char *c = grid_get_cell(grid,i,j);
      if(c == NULL) 
        return;

      size_t n = 0;
      while((n < grid->size) && (c[n] == color_table[n])) n++;

      if(n == grid->size)
      {
        fprintf(fd, "%c ", EMPTY_CELL);
      }
      else
        fprintf(fd, "%s ", c); 
      
      free(c);
      
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

char *grid_get_cell(const grid_t *grid, const size_t row, const size_t column)
{
  if(grid == 0 || row >= grid-> size || column >= grid->size) return NULL;

  colors_t cell = grid->cells[row][column];
  size_t count = colors_count(cell);

  char *cell_str = malloc ((count + 1) * sizeof(char));
  if(cell_str == NULL) 
    return NULL;
  
  
  size_t i = 0;
  size_t color_id = 0;
  while (i < count )
  {
    if(colors_is_in(cell,color_id))
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
  
  // lines subgrids 

  colors_t **lines = NULL;
  lines = malloc (grid->size * sizeof(colors_t*));
  if(lines == NULL) 
    goto error;
  
  for(size_t i = 0; i < grid->size ; i++)
  {
    colors_t *subgrid = NULL;
    subgrid = malloc (grid->size * sizeof(colors_t));
    if(subgrid == NULL) 
      goto error;
    
    for(size_t j = 0; j < grid->size; j++)
      subgrid[j] = grid->cells[i][j];

    lines[i] = subgrid;
  }

  // columns subgrids 

  colors_t **columns = NULL;
  columns = malloc (grid->size * sizeof(colors_t*));
  if(columns == NULL) 
    goto error;

  for(size_t j = 0; j < grid->size ; j++)
  {
    colors_t *subgrid = NULL;
    subgrid = malloc (grid->size * sizeof(colors_t));
    if(subgrid == NULL) 
      goto error;
    
    for(size_t i = 0; i < grid->size; i++)
      subgrid[i] = grid->cells[i][j]; 

    columns[j] = subgrid;
  }

  // square root calculation
  
  float sqrt = (grid->size == 1)? 1 : grid->size / 2;
  float tmp = 0;
  while(sqrt != tmp)
  {
    tmp = sqrt;
    sqrt = (grid->size / tmp + tmp) / 2;
  }
  
  // blocks subgrids 

  colors_t **blocks = NULL;
  blocks = malloc (grid->size * sizeof(colors_t*));
  if(blocks == NULL) 
    goto error;

  int n = 0;
  
  for(size_t i = 0; i < (size_t)sqrt; i++ )
  {
    for(size_t j = 0; j < (size_t)sqrt; j++)
    {
      colors_t *subgrid = NULL;
      subgrid = malloc (grid->size * sizeof(colors_t));
      if(subgrid == NULL) 
        goto error;

      int m = 0;

      for(size_t k = 0; k < (size_t)sqrt; k++)
        for(size_t l = 0; l < (size_t)sqrt; l++)
          subgrid[m++] = grid->cells[i*(size_t)sqrt +k][ j*(size_t)sqrt+l];

      blocks[n++] = subgrid;
    }
  }


  for(size_t i = 0; i < grid->size; i++)
  {
    if(!subgrid_consistency(lines[i],grid->size) ||
        !subgrid_consistency(columns[i],grid->size) ||
          !subgrid_consistency(blocks[i],grid->size))
    {
    error :

      for(size_t i = 0; i < grid->size; i++)
      {
        free(lines[i]);
        free(columns[i]);
        free(blocks[i]);
      }
      free(lines);
      free(columns);
      free(blocks);
      return false;
    }

  }
  for(size_t i = 0; i < grid->size; i++)
  {
    free(lines[i]);
    free(columns[i]);
    free(blocks[i]);
  }
  free(lines);
  free(columns);
  free(blocks);
  return true;
}

size_t grid_heuristics(grid_t *grid)
{
  bool keep_going = true;

// lines subgrids 

  colors_t ***lines = NULL;
  lines = malloc (grid->size * sizeof(colors_t**));
  if(lines == NULL) 
    goto error;

  for(size_t i = 0; i < grid->size; i++)
    {
      colors_t **subgrid = NULL;
      subgrid = malloc (grid->size * sizeof(colors_t*));
      if(subgrid == NULL) 
        goto error;
      
      for(size_t j = 0; j < grid->size; j++)
        subgrid[j] = &grid->cells[i][j];
      
      lines[i] = subgrid;
    }
  
  // columns subgrids 

  colors_t ***columns = NULL;
  columns = malloc (grid->size * sizeof(colors_t**));
  if(columns == NULL) 
    goto error;

  for(size_t j = 0; j < grid->size ; j++)
  {
    colors_t **subgrid = NULL;
    subgrid = malloc (grid->size * sizeof(colors_t*));
    if(subgrid == NULL) 
      goto error;

    for(size_t i = 0; i < grid->size; i++)
      subgrid[i] = &grid->cells[i][j]; 

    columns[j] = subgrid;
  }
  
  // square root calculation
  
  float sqrt = (grid->size == 1)? 1 : grid->size / 2;
  float tmp = 0;
  while(sqrt != tmp)
  {
    tmp = sqrt;
    sqrt = (grid->size / tmp + tmp) / 2;
  }

  // blocks subgrids 

  colors_t ***blocks = NULL;
  blocks = malloc (grid->size * sizeof(colors_t**));
  if(blocks == NULL) 
    goto error;

  int n = 0;
  
  for(size_t i = 0; i < (size_t)sqrt; i++ )
  {
    for(size_t j = 0; j < (size_t)sqrt; j++)
    {
      colors_t **subgrid = NULL;
      subgrid = malloc (grid->size * sizeof(colors_t*));
      if(subgrid == NULL) 
        goto error;

      int m = 0;

      for(size_t k = 0; k < (size_t)sqrt; k++)
        for(size_t l = 0; l < (size_t)sqrt; l++)
          subgrid[m++] = &grid->cells[i*(size_t)sqrt +k][ j*(size_t)sqrt+l];

      blocks[n++] = subgrid;
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
      keep_going = keep_tmp || keep_going;
    }

    if(!grid_is_consistent(grid) || grid_is_solved(grid)) keep_going = false;
  }

  error: 
  
  for(size_t i = 0; i < grid->size; i++)
  {
    free(lines[i]);
    free(columns[i]);
    free(blocks[i]);
  }

  free(lines);
  free(columns);
  free(blocks);

  return (grid_is_consistent(grid))? ((grid_is_solved(grid))? 1 : 0) : 2;
}
