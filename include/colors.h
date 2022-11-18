#ifndef COLORS_H
#define COLORS_H

#define MAX_COLORS 64

#include <stdbool.h>
#include <stdint.h> 
#include <stdlib.h>

typedef uint64_t colors_t;

/* initialize a colors_t with all colors */
colors_t colors_full (const size_t size);

/* initialize a colors_t with no color */
colors_t colors_empty (void);

/* initialize a colors_t with a color to the given index */
colors_t colors_set (const size_t color_id);

/* add the color of index color_id to colors */
colors_t colors_add (const colors_t colors, const size_t color_id);

/* remove the color of index color_id from colors */
colors_t colors_discard (const colors_t colors, const size_t colord_id);

/* checks if the color of the index color_id is in colors */
bool colors_is_in (const colors_t colors, const size_t colord_id);

/* bitwise negates the colors of colors */
colors_t colors_negate (const colors_t colors);

/* computes the intersection between colors1 and colors2 */
colors_t colors_and (const colors_t colors1, const colors_t colors2);

/* computes the inclusive union between colors1 and colors2 */
colors_t colors_or (const colors_t colors1, const colors_t colors2);

/* computes the exclusive union between colors1 and colors2 */
colors_t colors_xor (const colors_t colors1, const colors_t colors2);

/* computes the substraction between colors1 and colors2 */
colors_t colors_subtract (const colors_t colors1, const colors_t colors2);

/* checks if colors1 and colors2 are the same colors */
bool colors_is_equal (const colors_t colors1, const colors_t colors2);

/* checks the inclusion of colors1 in colors2 */
bool colors_is_subset (const colors_t colors1, const colors_t colors2);

/* checks if there is only one color in the given colors */
bool colors_is_singleton (const colors_t colors);

/* count the number of colors in the given colors */
size_t colors_count (const colors_t colors);

/* returns the rightmost color of the given colors */
colors_t colors_rightmost (const colors_t colors);

/* returns the leftmost color of the given colors */
colors_t colors_leftmost (const colors_t colors);

/* returns a random color of the given colors */
colors_t colors_random (const colors_t colors);

/* checks if a subgrid is consistent */
bool subgrid_consistency(colors_t subgrid[], const size_t size);

/* applies heuristics on given subgrid */
bool subgrid_heuristics(colors_t *subgrid[], size_t size);

#endif /* COLORS_H*/