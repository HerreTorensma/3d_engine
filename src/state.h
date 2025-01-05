#pragma once

#include "engine/global.h"
#include "engine/core.h"
#include "entity.h"

// Contains the entire state of the game in one convinient place
typedef struct state {
    index_t player_ent_index;
    res_pack_t res_pack;
    grid_t grid;
    index_t mode;
	SDL_Window *window;
	bool cursor_free;
	ent_system_t ent_system;
	bool edit_mode;
} state_t;

extern state_t state;