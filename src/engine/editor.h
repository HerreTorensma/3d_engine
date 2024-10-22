#pragma once

#include "global.h"
#include "util.h"
#include "res.h"
#include "level.h"
#include "shader.h"
#include "input.h"
#include "render.h"

void editor_init(void);

void editor_update(level_t *level);

void editor_render(res_pack_t *res_pack, level_t *level);

void editor_input(SDL_Event event);