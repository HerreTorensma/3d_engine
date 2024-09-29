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
    MESH_BIG_QUAD = 1,
    MESH_CUBE = 2,
    MESH_SLOPE = 3,
    MESH_FLOOR = 4,
    MESH_CROSS = 5,
    MESH_QUAD = 6,
};