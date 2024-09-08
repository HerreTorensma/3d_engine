#pragma once

#include "global.h"
#include "shader.h"
#include "util.h"
#include "input.h"
#include "level.h"
#include "res.h"

// typedef struct renderer {

// } renderer_t;

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;

    f32 pitch;
    f32 yaw;
} camera_t;

void render_init();

void render();

void render_input(SDL_Event event);