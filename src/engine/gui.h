/*
This header and source file contains the immediate-mode gui library made for this engine.
*/
#pragma once

#include "global.h"
#include "res.h"
#include "render.h"
#include "input.h"
#include "core.h"

void font_init(font_t *font, res_pack_t *res_pack, size_t texture_index);

void gui_print(res_pack_t *res_pack, font_t *font, const char text[], i32 x, i32 y, color_t color);

bool gui_button(res_pack_t *res_pack, const char text[], rect_t tile_rect);

void gui_text_edit(res_pack_t *res_pack, char text[], size_t buffer_size, rect_t tile_rect);

void gui_id_reset(void);

u32 gui_id_gen(void);