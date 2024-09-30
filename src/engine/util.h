#pragma once

#include "global.h"
#include "mesh.h"

extern i32 window_width;
extern i32 window_height;

typedef struct image {
    u16 width;
    u16 height;
    u8 *pixels;
} image_t;

typedef struct color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} color_t;

typedef struct gl_color {
    f32 r;
    f32 g;
    f32 b;
    f32 a;
} gl_color_t;

char *load_file_to_string(const char path[]);

image_t load_tga(const char path[], bool flip);

mesh_t load_mesh(const char path[]);

gl_color_t color_to_gl_color(color_t color);