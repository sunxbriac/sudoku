#define MAX_COLORS 64

#include <stdbool.h>
#include <stdint.h> 
#include <stdlib.h>

typedef uint64_t colors_t;

colors_t colors_full (const size_t size);
/* sets 1 to all bits within range from 0 to size and 0 to others */

colors_t colors_empty (void);
/* initialize colors with only 0s */

colors_t colors_set (const size_t color_id);
/* initialize colors with 1 to the given index and 0 to others */

colors_t colors_add (const colors_t colors, const size_t color_id);
/* sets the given index of colors to 1 */

colors_t colors_discard (const colors_t colors, const size_t colord_id);
/* sets the given index of colors to 0 */

bool colors_is_in (const colors_t colors, const size_t colord_id);
/* checks if the color of the given index is 1 in colors */

colors_t colors_negate (const colors_t colors);
/* bitwise negates the colors of colors */

colors_t colors_and (const colors_t colors1, const colors_t colors2);
/* computes the intersection between colors1 and colors2 */

colors_t colors_or (const colors_t colors1, const colors_t colors2);
/* computes the inclusive union between colors1 and colors2 */

colors_t colors_xor (const colors_t colors1, const colors_t colors2);
/* computes the exclusive union between colors1 and colors2 */

colors_t colors_subtract (const colors_t colors1, const colors_t colors2);
/* computes the substraction between colors1 and colors2 */

bool colors_is_equal (const colors_t colors1, const colors_t colors2);
/* checks if colors1 and colors2 are the same colors */

bool colors_is_subset (const colors_t colors1, const colors_t colors2);
/* checks the inclusion of colors1 in colors2 */

bool colors_is_singleton (const colors_t colors);
/* checks if there is only one color in the given colors */

size_t colors_count (const colors_t colors);
/* count the number of colors in the given colors */

colors_t colors_rightmost (const colors_t colors);
/* returns the rightmost color of the given colors */

colors_t colors_leftmost (const colors_t colors);
/* returns the leftmost color of the given colors */

colors_t colors_random (const colors_t colors);
/* returns a random color of the given colors */





