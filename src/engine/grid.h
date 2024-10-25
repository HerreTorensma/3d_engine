#pragma once

#include "global.h"

typedef struct grid {
	u32 width;
	u32 height;
	u32 depth;

	u64 map[4096];
} grid_t;

inline size_t grid_get_cell(grid_t *level, u32 x, u32 y, u32 z) {
	return level->map[z * level->width * level->height + y * level->width + x];
}

inline void grid_set_cell(grid_t *level, size_t index, u32 x, u32 y, u32 z) {
	level->map[z * level->width * level->height + y * level->width + x] = index;
}