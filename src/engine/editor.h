#pragma once

#include "global.h"
#include "util.h"
#include "res.h"
#include "shader.h"
#include "input.h"
#include "render.h"
#include "gui.h"
#include "grid.h"

void editor_init(void);

void editor_update(res_pack_t *res_pack, grid_t *level);

void editor_render(res_pack_t *res_pack, grid_t *grid, i32 mesh_amount, i32 tex_amount);

void editor_input(SDL_Event event);