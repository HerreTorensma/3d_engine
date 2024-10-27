#pragma once

#include "global.h"
#include "shader.h"
#include "util.h"
#include "input.h"
#include "grid.h"
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

void render_game(res_pack_t *res_pack, grid_t *grid, ecs_world_t *ecs, camera_t *camera);

void render_grid_ortho(res_pack_t *res_pack, grid_t *grid, enum ortho_view orientation, float zoom, mat4 *projection);
// void render_grid_ortho(res_pack_t *res_pack, grid_t *grid, enum ortho_view orientation, float zoom);

void render_image(res_pack_t *res_pack, enum pivot pivot, size_t texture_index, i32 x, i32 y);
// void render_image(res_pack_t *res_pack, enum pivot pivot, size_t texture_index, i32 x, i32 y);

void render_image_ex(res_pack_t *res_pack, size_t texture_index, enum pivot pivot, rect_t src, i32 x, i32 y, float rotation, vec2 scale);

void render_start_frame_buffer(res_pack_t *res_pack);

void render_end_frame_buffer(res_pack_t *res_pack);

void render_mesh_isometric(res_pack_t *res_pack, mesh_t mesh, size_t texture_index, i32 x, i32 y);