#pragma once

#include "global.h"
#include "core.h"

bool point_overlap_box(vec3 point, vec3 box_pos, box_t box);

bool box_overlap_box(vec3 box1_pos, box_t box1, vec3 box2_pos, box_t box2);

collision_t check_player_collision(res_pack_t *res_pack, grid_t *grid, camera_t *camera, box_t *player_box);