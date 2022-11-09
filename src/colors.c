#include "colors.h"

#include <stdio.h>
#include <unistd.h>

#include <time.h>


colors_t colors_full (const size_t size)
{
  if(size == 0) return (colors_t)0;
  if(size < MAX_COLORS) return (0xFFFFFFFFFFFFFFFF >> (MAX_COLORS - size));
  return 0xFFFFFFFFFFFFFFFF;
}


colors_t colors_empty (void)
{
  return (colors_t)0;
}


colors_t colors_set (const size_t color_id)
{
  if(color_id >= MAX_COLORS) return 0;

  colors_t temp = 0x1;
  return (temp << (color_id));
}


colors_t colors_add (const colors_t colors, const size_t colors_id)
{
  return colors | colors_set(colors_id);
}


colors_t colors_discard (const colors_t colors, const size_t colors_id)
{
  colors_t modif_id = colors ^ colors_set(colors_id);
  return (modif_id < colors) ? modif_id : colors;
}


bool colors_is_in (const colors_t colors, const size_t colors_id)
{
  return !(colors == colors_discard (colors,colors_id));
}


colors_t colors_negate (const colors_t colors)
{
  return ~colors;
}


colors_t colors_and (const colors_t colors1, const colors_t colors2)
{
  return colors1 & colors2;
}


colors_t colors_or (const colors_t colors1, const colors_t colors2)
{
  return colors1 | colors2;
}


colors_t colors_xor (const colors_t colors1, const colors_t colors2)
{
  return colors1 ^ colors2;
}


colors_t colors_subtract (const colors_t colors1, const colors_t colors2)
{
  return colors1 & ~colors2;
}


bool colors_is_equal (const colors_t colors1, const colors_t colors2)
{
  return (colors_xor (colors1,colors2) == 0);
}


bool colors_is_subset (const colors_t colors1, const colors_t colors2)
{
  return colors_is_equal (colors1, colors_and (colors1, colors2));
}


bool colors_is_singleton (const colors_t colors)
{
  if(colors_is_equal (colors, 0)) return false;
  return colors_is_equal (colors_and (colors, colors - (colors_t)1), 0);
}


size_t colors_count (const colors_t colors)
{
  colors_t mask5 = colors_full (32);
  colors_t mask4 = colors_xor (mask5, mask5 << 16);
  colors_t mask3 = colors_xor (mask4, mask4 << 8);
  colors_t mask2 = colors_xor (mask3, mask3 << 4);
  colors_t mask1 = colors_xor (mask2, mask2 << 2);
  colors_t mask0 = colors_xor (mask1, mask1 << 1);

  colors_t size = colors_and (colors >> 1, mask0) + colors_and (colors, mask0);
  size = colors_and (size >> 2, mask1) + colors_and (size, mask1);
  size = colors_and (size >> 4, mask2) + colors_and (size, mask2);
  size = colors_and (size >> 8, mask3) + colors_and (size, mask3);
  size = colors_and (size >> 16, mask4) + colors_and (size, mask4);
  size = colors_and (size >> 32, mask5) + colors_and (size, mask5);

  return (size_t)size;
}


colors_t colors_rightmost (const colors_t colors)
{
  if(colors_is_equal (colors, colors_empty())) return colors;
  return colors_and (colors, -colors);
}


colors_t colors_leftmost (const colors_t colors)
{
  if(colors_is_singleton (colors)) return colors_rightmost(colors);
  
  colors_t temp_colors = colors_or (colors, colors >> 1);
  temp_colors = colors_or (temp_colors, temp_colors >> 2);
  temp_colors = colors_or (temp_colors, temp_colors >> 4);
  temp_colors = colors_or (temp_colors, temp_colors >> 8);
  temp_colors = colors_or (temp_colors, temp_colors >> 16);
  temp_colors = colors_or (temp_colors, temp_colors >> 32);
  temp_colors++;

  if(colors_is_equal (temp_colors,0)) return colors_set(MAX_COLORS - 1);
  return (colors_rightmost (temp_colors) >> 1);
}


colors_t colors_random (const colors_t colors)
{
  if(colors == 0) return 0;
  colors_t colors_copy = colors;
  colors_t colors_res = colors_rightmost (colors_copy);

  static bool seeded = false;
  if(!seeded) {
    srand (time (NULL));
    seeded = true;
  }
  
  int rand_id = rand() %colors_count (colors);
  int i = 1;

  while(i < rand_id)
  {
    colors_res = colors_rightmost (colors_copy); 
    colors_copy = colors_subtract (colors_copy,colors_res);
    i++;
  }
  
  return colors_res;
}


bool subgrid_consistency(colors_t subgrid[], const size_t size)
{
  colors_t single = colors_empty();
  size_t single_count = 0;
  colors_t full = colors_empty();

  for(size_t i = 0; i < size; i++)
  {
    if(colors_is_equal (subgrid[i],colors_empty())) 
      return false;
    if(colors_is_singleton (subgrid[i])) 
    {
      single = colors_or (single,subgrid[i]);
      single_count++;
    }
    full = colors_or (full,subgrid[i]);
  }

  if(single_count != colors_count (single)) return false;
  return (colors_is_equal (full, colors_full(size)));
}


bool subgrid_heuristics(colors_t *subgrid[], size_t size)
{
  bool change = false;

  // cross-hatching 

  for(size_t i = 0; i < size; i++)
  {
    if(colors_is_singleton (*subgrid[i]))
    {
      for(size_t j = 0; j < size; j++)
      {
        colors_t temp_colors = *subgrid[j] & ~*subgrid[i];
        if((*subgrid[j] ^ temp_colors) != 0 && i != j)
      {
        change = true;
        *subgrid[j] = temp_colors;
      }
      }
    }
  }
  
  // lone number 

  colors_t colors_in_multi = colors_empty();
  colors_t colors_in = colors_empty();
  
  for(size_t i = 0; i < size; i++)
  {
    colors_t colors_alrdy_in = colors_in & *subgrid[i];
    colors_in_multi = colors_alrdy_in | colors_in_multi;
    colors_in = colors_in | *subgrid[i];
  }
  
  if(!colors_is_equal (colors_full(size), colors_in_multi))
  {
    colors_t colors_single = ~colors_in_multi;

    for(size_t i = 0; i < size; i++)
    {
      colors_t colors_tmp = colors_single & *subgrid[i];
      if(!colors_is_singleton (*subgrid[i]) && 
          ((colors_tmp) != 0))
      {
        *subgrid[i] = colors_tmp;
        change = true;
      }
    }
  }

  // naked-subset
  
  for(size_t i = 0; i < size; i++)
  {
    colors_t subset = *subgrid[i];
    if(!colors_is_singleton (subset))
    {
      size_t count = colors_count (subset);
      size_t n = 0;

      for(size_t j = 0; j < size; j++)
        if(colors_is_subset (*subgrid[j], subset))
          n++;

      if(n == count)
      {
        for(size_t j = 0; j < size; j++)
        {
          if(!colors_is_subset (*subgrid[j], subset))
          {
            colors_t colors_tmp = *subgrid[j] & ~subset;
            if(!colors_is_equal (colors_tmp, *subgrid[j]))
            {
              change = true;
              *subgrid[j] = colors_tmp;
            }
          }
        }
      }
    }
  }

  // hidden-subset not functional

/*
  colors_t *colors_index = NULL;
  colors_index = malloc(size * sizeof(colors_t));
  if(colors_index == NULL) return false;

  for(size_t i = 0; i < size; i++)  
    colors_index[i] = colors_empty();


  for(size_t i = 0; i < size; i ++)
    for(size_t j = 0; j < size; j++)
      if(colors_is_in (*subgrid[i], j))
        colors_add (colors_index[j], i);
  


  for(size_t i = 0; i < size; i++)
  {
    for(size_t j = 0; j < size; j++)
    {
      colors_t uni = colors_index[i] | colors_index[j];
      size_t count = colors_count (uni);
      size_t n = 0;

      size_t *included_index = NULL;
      included_index = malloc (size * sizeof(size_t));
      if(included_index == NULL) 
        return false;

      for(size_t k = 0; k < size; k++)
      {
        if(colors_is_subset (colors_index[k], uni))
        {
          included_index[n] = k;
          n++;
        }
      }

      if(n == count)
      {
        colors_t hidden_subset = colors_empty();

        for(size_t l = 0; l < count; l++)
          colors_add(hidden_subset, included_index[l]);

        for(size_t k = 0; k < size; k++)
        {
          if(colors_is_in (uni, k) && 
            !colors_is_equal ((hidden_subset & *subgrid[k]), *subgrid[k]))
            {
            printf("going in hidden\n");
            change = true;
            *subgrid[k] = hidden_subset;
            }
        }
      }

      free (included_index);
    }
  }

  free (colors_index);
  */
  
  return change;
}