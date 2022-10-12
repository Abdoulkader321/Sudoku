#include "colors.h"

#include <stdio.h>

#include <time.h>

#define ULONG_MAX 0xffffffff

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

bool colors_is_subset(const colors_t colors1, const colors_t colors2) {

  return (colors2 | colors1) == colors2;
}

colors_t colors_substract(const colors_t colors1, const colors_t colors2) {

  return colors_xor(colors1, colors2);
}

colors_t colors_rightmost(const colors_t colors) {

  return colors & colors_negate(colors - 1);
}

colors_t colors_leftmost(const colors_t colors) {

  if (colors == 0) {
    return 0;
  }

  int pos = -1;

  colors_t our_colors = colors;

  while (our_colors != 0) {
    our_colors >>= 1;
    pos++;
  }

  return colors_set(pos);
}

colors_t colors_random(const colors_t colors) {

  if (colors == 0) {
    return 0;
  }

  size_t nb_colors = colors_count(colors);

  srand(time(NULL));
  size_t random_color = (rand() % nb_colors) + 1;

  int pos = -1;

  while (random_color != 0) {

    pos++;

    if (colors_is_in(colors, pos)) {
      random_color--;
    }
  }

  return colors_set(pos);
}

int main(void) { return EXIT_SUCCESS; }