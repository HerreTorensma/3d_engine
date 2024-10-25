#pragma once

#include "global.h"
#include "util.h"
#include "res.h"
#include "grid.h"
#include "shader.h"
#include "input.h"
#include "render.h"

void editor_init(void);

void editor_update(grid_t *level);

void editor_render(res_pack_t *res_pack, grid_t *level);

void editor_input(SDL_Event event);