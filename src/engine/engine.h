#pragma once

#include "global.h"
#include "input.h"
#include "memory.h"
#include "util.h"

void engine_init(const char window_title[], i32 width, i32 height, i32 render_width, i32 render_height, void (*input_function)(SDL_Event event));

void engine_update();

void engine_update_end();

bool window_should_close();

void engine_quit();
