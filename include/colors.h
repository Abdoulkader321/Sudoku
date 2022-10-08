#ifndef COLORS_H
#define COLORS_H

#define MAX_COLORS 64

#include <stdint.h>
#include <stdlib.h>

typedef uint64_t colors_t;

colors_t colors_full(const size_t size);
colors_t colors_empty(void);
colors_t colors_set(const size_t color_id);
colors_t colors_add(const colors_t colors, const size_t color_id);
colors_t colors_discard(const colors_t colors, const size_t color_id);

#endif /* COLORS_H */