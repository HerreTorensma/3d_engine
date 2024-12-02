#include "collision.h"

bool point_overlap_box(vec3 point, vec3 box_pos, box_t box) {
	box.min_x += box_pos[0];
	box.max_x += box_pos[0];
	box.min_y += box_pos[1];
	box.max_y += box_pos[1];
	box.min_z += box_pos[2];
	box.max_z += box_pos[2];

	if (point[0] >= box.min_x &&
		point[0] <= box.max_x &&
		
		point[1] >= box.min_y &&
		point[1] <= box.max_y &&
		
		point[2] >= box.min_z &&
		point[2] <= box.max_z
	) {
		return true;
	}

	return false;
}

bool box_overlap_box(vec3 box1_pos, box_t box1, vec3 box2_pos, box_t box2) {
	box1.min_x += box1_pos[0];
	box1.max_x += box1_pos[0];
	box1.min_y += box1_pos[1];
	box1.max_y += box1_pos[1];
	box1.min_z += box1_pos[2];
	box1.max_z += box1_pos[2];

	box2.min_x += box2_pos[0];
	box2.max_x += box2_pos[0];
	box2.min_y += box2_pos[1];
	box2.max_y += box2_pos[1];
	box2.min_z += box2_pos[2];
	box2.max_z += box2_pos[2];

	if (
		box1.min_x <= box2.max_x &&
		box1.max_x >= box2.min_x &&

		box1.min_y <= box2.max_y &&
		box1.max_y >= box2.min_y &&

		box1.min_z <= box2.max_z &&
		box1.max_z >= box2.min_z
	) {
		return true;
	}

	return false;
}

collision_t check_player_collision(res_pack_t *res_pack, grid_t *grid, camera_t *camera, box_t *player_box) {
	// Get all 8 map positions surrounding the player, given the player is smaller than a map cell
	vec3 map_positions[] = {
		{
			roundf(camera->position[0] + player_box->min_x),
			roundf(camera->position[1] + player_box->min_y),
			roundf(camera->position[2] + player_box->min_z),
		},
		{
			roundf(camera->position[0] + player_box->max_x),
			roundf(camera->position[1] + player_box->min_y),
			roundf(camera->position[2] + player_box->min_z),
		},
		{
			roundf(camera->position[0] + player_box->max_x),
			roundf(camera->position[1] + player_box->max_y),
			roundf(camera->position[2] + player_box->min_z),
		},
		{
			roundf(camera->position[0] + player_box->max_x),
			roundf(camera->position[1] + player_box->max_y),
			roundf(camera->position[2] + player_box->max_z),
		},
		{
			roundf(camera->position[0] + player_box->min_x),
			roundf(camera->position[1] + player_box->max_y),
			roundf(camera->position[2] + player_box->min_z),
		},
		{
			roundf(camera->position[0] + player_box->max_x),
			roundf(camera->position[1] + player_box->min_y),
			roundf(camera->position[2] + player_box->max_z),
		},
		{
			roundf(camera->position[0] + player_box->min_x),
			roundf(camera->position[1] + player_box->max_y),
			roundf(camera->position[2] + player_box->max_z),
		},
		{
			roundf(camera->position[0] + player_box->min_x),
			roundf(camera->position[1] + player_box->min_y),
			roundf(camera->position[2] + player_box->max_z),
		},
	};

	for (i32 i = 0; i < 8; i++) {
		i32 map_x = (i32)(map_positions[i][0]);
		i32 map_y = (i32)(map_positions[i][1]);
		i32 map_z = (i32)(map_positions[i][2]);

		if (map_x < 0 || map_y < 0 || map_z < 0) {
			continue;
		}

		tile_t current_tile = grid_get_cell(grid, map_x, map_y, map_z);
		
		for (int j = 0; j < res_pack->meshes[current_tile.mesh_index].collider.boxes_len; j++) {
			box_t box = res_pack->meshes[current_tile.mesh_index].collider.boxes[j];
			
			if (box_overlap_box(camera->position, *player_box, map_positions[i], box)) {
				// colliding = true;
				// return &res_pack->meshes[current_tile.mesh_index].collision.boxes[j];
				box_t collision_box = box;

				collision_box.min_x += map_positions[i][0];
				collision_box.max_x += map_positions[i][0];
				collision_box.min_y += map_positions[i][1];
				collision_box.max_y += map_positions[i][1];
				collision_box.min_z += map_positions[i][2];
				collision_box.max_z += map_positions[i][2];

				collision_t collision = {
					.hit = true,
					.global_box = collision_box,
				};

				return collision;
			}
		}
	}

	return (collision_t){0};
}