#pragma once

#include "global.h"

typedef struct vec3i {
	i64 x;
	i64 y;
	i64 z;
} vec3i;

typedef struct level {
	u32 width;
	u32 height;
	u32 depth;

	u64 map[4096];

	vec3i transparent_tile_cells[256];
} level_t;

inline size_t level_get_tile_index(level_t *level, u32 x, u32 y, u32 z) {
	return level->map[z * level->width * level->height + y * level->width + x];
}

inline void level_set_tile_index(level_t *level, size_t index, u32 x, u32 y, u32 z) {
	level->map[z * level->width * level->height + y * level->width + x] = index;
}