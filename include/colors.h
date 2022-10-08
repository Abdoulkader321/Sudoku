#ifndef COLORS_H
#define COLORS_H

#define MAX_COLORS 64

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t colors_t;

colors_t colors_full(const size_t size);
colors_t colors_empty(void);
colors_t colors_set(const size_t color_id);
colors_t colors_add(const colors_t colors, const size_t color_id);
colors_t colors_discard(const colors_t colors, const size_t color_id);
bool colors_is_in(const colors_t colors, const size_t color_id);
colors_t colors_negate(const colors_t colors);
colors_t colors_and(const colors_t colors1, const colors_t colors2);
colors_t colors_or(const colors_t colors1, const colors_t colors2);
colors_t colors_xor(const colors_t colors1, const colors_t colors2);
colors_t colors_substract(const colors_t colors1, const colors_t colors2);

colors_t colors_is_equal(const colors_t colors1, const colors_t colors2);

#endif /* COLORS_H */