/*
This header and source file contain utility helper functions.
*/
#pragma once

#include "global.h"
#include "core.h"

#define DEBUG

extern i32 window_width;
extern i32 window_height;
extern i32 screen_scale;

char *load_file_to_string(const char path[]);

texture_t load_tga(const char path[], bool flip);

mesh_t load_mesh_raw(vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count);

mesh_t load_mesh(const char path[]);

void color_to_gl_color(color_t color, vec4 gl_color);

void get_mouse_pos(i32 *x, i32 *y);

void get_normalized_mouse_pos(float *x, float *y);

void debug_log(const char *format, ...);

box_t compute_bounding_box(mesh_t *mesh);

void print_box(box_t *box);

void print_mesh(res_pack_t *res_pack, index_t mesh_index);

inline i32 min_i32(i32 a, i32 b) {
	if (a < b) {
		return a;
	}
	return b;
}