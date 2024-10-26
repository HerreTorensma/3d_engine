#pragma once

#include "global.h"
#include "res.h"
#include "render.h"

typedef struct font {
    size_t texture_index;
    rect_t rects[256];
    u32 horizontal_spacing;
} font_t;

void font_init(font_t *font, res_pack_t *res_pack, size_t texture_index);

void gui_print(res_pack_t *res_pack, font_t *font, const char text[], i32 x, i32 y);