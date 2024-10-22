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
