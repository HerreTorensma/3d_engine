#pragma once

#include "global.h"
#include "mesh.h"

extern i32 window_width;
extern i32 window_height;

typedef struct color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} color_t;

char *load_file_to_string(const char path[]);

texture_t load_tga(const char path[], bool flip);

mesh_t load_mesh(const char path[]);

void color_to_gl_color(color_t color, vec4 gl_color);

void get_normalized_mouse_pos(float *x, float *y);