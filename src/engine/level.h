#pragma once

#include "global.h"

typedef struct level {
	u32 width;
	u32 height;
	u32 depth;

	u8 map[4096];
} level_t;

inline size_t level_get_mesh_index(level_t *level, u32 x, u32 y, u32 z) {
	return level->map[z * level->width * level->height + y * level->width + x];
}

inline void level_set_mesh_index(level_t *level, size_t index, u32 x, u32 y, u32 z) {
	level->map[z * level->width * level->height + y * level->width + x] = index;
}