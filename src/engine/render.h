#pragma once

#include "global.h"
#include "shader.h"
#include "util.h"
#include "input.h"
#include "level.h"
#include "res.h"

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;

    f32 pitch;
    f32 yaw;
} camera_t;

typedef struct atlas {
    u32 rows;
    u32 columns;
    
    u32 tex_width;
    u32 tex_height;

    u32 texture_id;
} atlas_t;

void render_init();

void render();

void render_input(SDL_Event event);