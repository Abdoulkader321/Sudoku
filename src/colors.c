#include "colors.h"

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

int main(void) {

  // printf("colors_full %lu\n", colors_full(6));

  // printf("colors_set %ld\n", colors_set(4));

  // printf("colors_add %ld\n", colors_add(12, 1));

  //printf("colors_discard %ld\n", colors_discard(31, 4));

  return 0;
}