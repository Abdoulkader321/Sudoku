#include "colors.h"

#include <stdio.h>

#include <time.h>

#define ULONG_MAX 0xffffffffffffffff

colors_t colors_full(const size_t size) {

  return (size >= MAX_COLORS) ? ULONG_MAX : (1UL << size) - 1;
}

colors_t colors_empty(void) { return 0; }

colors_t colors_set(const size_t color_id) {

  return color_id >= MAX_COLORS ? 0 : 1UL << color_id;
}

colors_t colors_add(const colors_t colors, const size_t color_id) {

  return colors | colors_set(color_id);
}

colors_t colors_discard(const colors_t colors, const size_t color_id) {

  return colors == 0 ? 0 : (colors & ~colors_set(color_id));
}

bool colors_is_in(const colors_t colors, const size_t color_id) {

  return (colors & colors_set(color_id)) != 0;
}

colors_t colors_negate(const colors_t colors) {

  return colors_xor(colors, ULONG_MAX);
}

colors_t colors_and(const colors_t colors1, const colors_t colors2) {

  return colors1 & colors2;
}

colors_t colors_or(const colors_t colors1, const colors_t colors2) {

  return colors1 | colors2;
}

colors_t colors_xor(const colors_t colors1, const colors_t colors2) {

  return colors1 ^ colors2;
}

colors_t colors_is_equal(const colors_t colors1, const colors_t colors2) {

  return colors1 == colors2;
}

bool colors_is_singleton(const colors_t colors) {

  return colors == 0 ? false : (colors & (colors - 1)) == 0;
}

size_t colors_count(const colors_t colors) {

  colors_t colors_count = colors;

  /* In binary, 0x5555555555555555 looks like ..0101 */
  colors_count = colors_count - ((colors_count >> 1) & 0x5555555555555555);

  /* 0x3333333333333333 looks like ..0011 */
  colors_count = (colors_count & 0x3333333333333333) +
                 ((colors_count >> 2) & 0x3333333333333333);

  /* 0x0F0F0F0F0F0F0F0F looks like ..00001111 */
  /* 0x0101010101010101 looks like ..000000001 */
  colors_count = (((colors_count + (colors_count >> 4)) & 0x0F0F0F0F0F0F0F0F) *
                  0x0101010101010101) >> 56;

  return colors_count;
}

bool colors_is_subset(const colors_t colors1, const colors_t colors2) {

  return colors1 == 0 ? 0 : (colors2 | colors1) == colors2;
}

colors_t colors_subtract(const colors_t colors1, const colors_t colors2) {

  return (colors1 ^ colors2) & colors1;
}

colors_t colors_rightmost(const colors_t colors) {

  return colors & colors_negate(colors - 1);
}

colors_t colors_leftmost(const colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t nb_colors = colors_count(colors);
  size_t nb_colors_found = 0;
  size_t pos = 0;

  while (nb_colors_found < nb_colors) {

    if (colors_is_in(colors, pos)) {
      nb_colors_found++;
    }

    pos++;
  }
  return colors_set(pos - 1);
}

colors_t colors_random(const colors_t colors) {

  if (colors == 0) {
    return colors_empty();
  }

  size_t nb_colors = colors_count(colors);

  srand(time(NULL));
  size_t random_color = (rand() % nb_colors) + 1;

  size_t pos = 0;

  while (random_color != 0) {

    if (colors_is_in(colors, pos)) {
      random_color--;
    }

    pos++;
  }

  return colors_set(pos - 1);
}
