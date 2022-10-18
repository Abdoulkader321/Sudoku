#ifndef COLORS_H
#define COLORS_H

#define MAX_COLORS 64

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t colors_t;

/* Initialize and return colors with size bits all set to '1' and all others to
 * '0' */
colors_t colors_full(const size_t size);

/* Returns zero */
colors_t colors_empty(void);

/* Set to '1' the color encoded at the index `color_id`, all others are zeros */
colors_t colors_set(const size_t color_id);

/* Set the given color index to '1' in colors and return it */
colors_t colors_add(const colors_t colors, const size_t color_id);

/* Set the given color index to '0' in colors and return it */
colors_t colors_discard(const colors_t colors, const size_t color_id);

/* Chech if the color index is set to '1' or not */
bool colors_is_in(const colors_t colors, const size_t color_id);

/* Bitwise negate the colors_t and return it */
colors_t colors_negate(const colors_t colors);

/* Compute the intersection between two colors_t and return it */
colors_t colors_and(const colors_t colors1, const colors_t colors2);

/* Compute the union between two colors_t and return it */
colors_t colors_or(const colors_t colors1, const colors_t colors2);

/* Compute the XOR between two colors_t and return it */
colors_t colors_xor(const colors_t colors1, const colors_t colors2);

/* Return colors1\colors2 */
colors_t colors_subtract(const colors_t colors1, const colors_t colors2);

/* Checks the equality of two colors_t */
colors_t colors_is_equal(const colors_t colors1, const colors_t colors2);

/* Test the inclusion of colors1 in colors2 */
bool colors_is_subset(const colors_t colors1, const colors_t colors2);

/* Checks if there is only one colors in the colors */
bool colors_is_singleton(const colors_t colors);

/* Return the numbers of colors enclosed in the set */
size_t colors_count(const colors_t colors);

/* Returns the rightmost color of the set (least significant bit) */
colors_t colors_rightmost(const colors_t colors);

/* Returns the rightmost color of the set (most significant bit) */
colors_t colors_leftmost(const colors_t colors);

/* Returns a random color chosen from the color set */
colors_t colors_random(const colors_t colors);

#endif /* COLORS_H */