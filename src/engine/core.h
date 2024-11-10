#pragma once

#include "global.h"

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

enum {
    MESH_QUAD = 1,
};

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

enum pivot {
	PIVOT_CENTER,
	PIVOT_TOP_LEFT,
};

typedef struct font {
    size_t texture_index;
    rect_t rects[256];
    u32 horizontal_spacing;
    i32 y_center;
} font_t;