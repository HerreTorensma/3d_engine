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

    // Offsets in the vbo
    // size_t vertex_offset;
    // size_t index_offset;

    u32 vao;
    u32 vbo;
    u32 ebo;
} mesh_t;