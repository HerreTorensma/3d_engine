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

void render_init(void);

void render_level(res_pack_t *res_pack, level_t *level, camera_t *camera);

void render2d_image(void);