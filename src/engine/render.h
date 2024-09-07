#pragma once

#include "global.h"
#include "shader.h"

typedef struct vertex {
    vec3 pos;
    vec2 tex_coord;
} vertex_t;

typedef struct mesh {
	vertex_t *vertices;
} mesh_t;

typedef struct camera {
    vec3 pos;
    vec3 front;
    vec3 up;

    f32 pitch;
    f32 yaw;
} camera_t;

void render_init();

void render();
