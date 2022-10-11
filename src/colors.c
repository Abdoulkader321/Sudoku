#include "colors.h"

#include <stdbool.h>
#include <stdio.h>

#include <limits.h>

colors_t colors_full(const size_t size) {

  return (size == 0 || size > MAX_COLORS) ? 0 : ~(ULONG_MAX << size);
}

colors_t colors_empty(void) { return 0; }

colors_t colors_set(const size_t color_id) {

  return color_id >= MAX_COLORS ? 0 : 1 << color_id;
}

colors_t colors_add(const colors_t colors, const size_t color_id) {

  return (colors | colors_set(color_id));
}

colors_t colors_discard(const colors_t colors, const size_t color_id) {

  return (colors ^ colors_set(color_id));
}

bool colors_is_in(const colors_t colors, const size_t color_id) {

  return colors & (1 << color_id);
}

colors_t colors_negate(const colors_t colors) { return ~colors; }

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

  /** Any power of 2 when subtracted by 1 is all 1 (Source: stackoverflow) */
  return colors == 0 ? false : (colors & (colors - 1)) == 0;
}

size_t colors_count(const colors_t colors) {

  size_t count = 0;

  colors_t our_colors = colors;

  while (our_colors != 0) {
    count += our_colors & 1; /**looking the first bit*/
    our_colors >>= 1;
  }

  return count;
}

int main(void) {

  printf("colors_count %ld", colors_count(12));

  return EXIT_SUCCESS;
}