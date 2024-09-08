#pragma once

#include "global.h"

extern i32 window_width;
extern i32 window_height;

typedef struct image {
    u16 width;
    u16 height;
    u8 *pixels;
} image_t;

char *load_file_to_string(const char path[]);

image_t load_tga(const char path[]);