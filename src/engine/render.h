/*
This header and source file contain the rendering code.
*/
#pragma once

#include "global.h"
#include "shader.h"
#include "util.h"
#include "input.h"
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

void render_start_frame_buffer(res_pack_t *res_pack);

void render_end_frame_buffer(res_pack_t *res_pack);

void render_image(res_pack_t *res_pack, size_t texture_index, i32 x, i32 y, color_t color);

void render_image_rect(res_pack_t *res_pack, size_t texture_index, rect_t src, rect_t dst, color_t color);

void render_mesh_isometric(res_pack_t *res_pack, mesh_t mesh, size_t texture_index, i32 x, i32 y, float scale);

void render_grid_layers_ortho(res_pack_t *res_pack, grid_t *grid, enum ortho_view orientation, float zoom, mat4 *projection, i32 min_y, i32 max_y);

void render_game(res_pack_t *res_pack, grid_t *grid, ecs_world_t *ecs, camera_t *camera);