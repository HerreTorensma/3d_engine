/*
This header contains the most important structs used by the engine,
conveniently in one file to avoid cyclic imports.
*/
#pragma once

#include "global.h"

// Structs
typedef struct vertex {
    vec3 position;
    vec2 tex_coord;
} vertex_t;

typedef struct mesh {
	vertex_t *vertices;
    u32 *indices;

    u32 vertex_count;
    u32 index_count;

    u32 vao;
    u32 vbo;
    u32 ebo;
} mesh_t;

typedef struct texture {
    u16 width;
    u16 height;
    u8 *pixels;

	u32 id;
} texture_t;

typedef struct rect {
	i32 x;
	i32 y;
	i32 w;
	i32 h;
} rect_t;

typedef struct color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} color_t;

typedef struct font {
    size_t texture_index;
    rect_t rects[256];
    u32 horizontal_spacing;
    i32 y_center;
    color_t color;
} font_t;

typedef struct theme {
	font_t font;
	size_t button_index;
	size_t button_pressed_index;
	u32 tile_size;
} theme_t;

typedef struct grid {
	u32 width;
	u32 height;
	u32 depth;

	u64 map[4096];
} grid_t;

inline size_t grid_get_cell(grid_t *level, u32 x, u32 y, u32 z) {
	return level->map[z * level->width * level->height + y * level->width + x];
}

inline void grid_set_cell(grid_t *level, size_t index, u32 x, u32 y, u32 z) {
	level->map[z * level->width * level->height + y * level->width + x] = index;
}

typedef struct tile {
	size_t mesh_index;
	size_t texture_index;
	vec3 rotation;
} tile_t;

typedef struct res_pack {
	color_t fog_color;
	color_t sky_color;
	color_t editor_color;

	u32 render_width;
	u32 render_height;

    mesh_t meshes[256];
	texture_t textures[256];
	size_t button_tex_index;
	size_t button_pressed_tex_index;
	u32 gui_tile_size;

	tile_t tiles[256];

	font_t font;
} res_pack_t;

// Core definitions
#define COLOR_WHITE (color_t){255, 255, 255, 255}
#define COLOR_BLACK (color_t){0, 0, 0, 255}
#define COLOR_RED (color_t){255, 0, 0, 255}
#define COLOR_GREEN (color_t){0, 255, 0, 255}
#define COLOR_BLUE (color_t){0, 0, 0, 255}

// Core enums
enum {
    MESH_QUAD = 1,
};

enum screen_anchor {
    ANCHOR_CENTER,
    ANCHOR_TOP,
    ANCHOR_BOTTOM,
    ANCHOR_LEFT,
    ANCHOR_RIGHT,
    ANCHOR_TOPLEFT,
    ANCHOR_TOPRIGHT,
    ANCHOR_BOTTOMLEFT,
    ANCHOR_BOTTOMRIGHT,
};

enum ortho_view {
    ORTHO_TOP,
    ORTHO_BOTTOM,
    ORTHO_FRONT,
    ORTHO_BACK,
    ORTHO_LEFT,
    ORTHO_RIGHT,
};

// Core components required by the engine itself
typedef struct transform {
	vec3 position;
	vec3 rotation;
	vec3 scale;
} transform_t;

typedef transform_t transform_c;

typedef struct {
	size_t texture_index;
	bool billboard;

    float x_scale;
    float y_scale;
} sprite_c;

typedef struct {
    size_t mesh_index;
    size_t texture_index;
} mesh_c;