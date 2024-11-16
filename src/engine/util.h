/*
This header and source file contain utility helper functions.
*/
#pragma once

#include "global.h"
#include "core.h"

extern i32 window_width;
extern i32 window_height;

char *load_file_to_string(const char path[]);

texture_t load_tga(const char path[], bool flip);

mesh_t load_mesh(const char path[]);

void color_to_gl_color(color_t color, vec4 gl_color);

void get_mouse_pos(i32 *x, i32 *y);

void get_normalized_mouse_pos(float *x, float *y);

void debug_log(const char *format, ...);