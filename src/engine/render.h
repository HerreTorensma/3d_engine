#pragma once

#include "global.h"
#include "shader.h"
#include "util.h"
#include "input.h"
#include "level.h"
#include "res.h"
#include "ecs.h"

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;

    f32 pitch;
    f32 yaw;
} camera_t;

typedef struct renderer {
    camera_t *camera;
    ecs_world_t *ecs;
} renderer_t;

void render_init(res_pack_t *res_pack);

void render_game(res_pack_t *res_pack, level_t *level, ecs_world_t *ecs, camera_t *camera);

void render_level_ortho(res_pack_t *res_pack, level_t *level, enum ortho_view orientation, float zoom);

void render2d_image(void);