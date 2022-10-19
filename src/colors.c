#include "colors.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>


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
  if (colors_is_equal (colors, 0)) return false;
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
  if (colors_is_equal (colors, colors_empty())) return colors;
  return colors_and (colors, -colors);
}


colors_t colors_leftmost (const colors_t colors)
{
  if (colors_is_singleton (colors)) return colors_rightmost(colors);
  
  colors_t temp_colors = colors_or (colors, colors >> 1);
  temp_colors = colors_or (temp_colors, temp_colors >> 2);
  temp_colors = colors_or (temp_colors, temp_colors >> 4);
  temp_colors = colors_or (temp_colors, temp_colors >> 8);
  temp_colors = colors_or (temp_colors, temp_colors >> 16);
  temp_colors = colors_or (temp_colors, temp_colors >> 32);
  temp_colors++;

  if (colors_is_equal (temp_colors,0)) return colors_set(MAX_COLORS - 1);
  return (colors_rightmost (temp_colors) >> 1);
}


colors_t colors_random (const colors_t colors)
{
  if(colors == 0) return 0;
  colors_t colors_copy = colors;
  colors_t colors_res = colors_rightmost (colors_copy);

  srand (time (NULL));
  int rand_id = rand() %colors_count(colors);
  int i = 1;

  while(i < rand_id)
  {
    colors_res = colors_rightmost (colors_copy); 
    colors_copy = colors_subtract (colors_copy,colors_res);
    i++;
  }
  
  return colors_res;
}












