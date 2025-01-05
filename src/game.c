#include "entity.h"
#include "engine/memory.h"
#include "engine/render.h"
#include "engine/res.h"
#include "engine/core.h"
#include "engine/collision.h"

#include "load_game.c"
#include "prefabs.c"
#include "state.h"

void player_controller(res_pack_t *res_pack, grid_t *grid, entity_t *player_ent) {
	if (input_key_pressed(SDL_SCANCODE_F)) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(state.window, window_width / 2, window_height / 2);
		state.cursor_free = true;
	}

	if (input_key_released(SDL_SCANCODE_F)) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		state.cursor_free = false;
	}

	transform_c *transform = &player_ent->transform;
	player_controller_c *controller = &player_ent->player_controller;
	player_collider_c *collider = &player_ent->player_collider;
	camera_c *camera = &player_ent->camera;
	
	if (input_key_pressed(SDL_SCANCODE_T)) {
		audio_play_sound_3d(res_pack, SOUND_JUMP, transform->position, camera->front, (vec3){10.0f, 1.0f, 10.0f});
	}

	float speed = controller->walk_speed;
	float gravity = -0.005f;

	if (input_key_held(SDL_SCANCODE_LSHIFT)) {
		speed = controller->crouch_speed;
	}

	vec2 input = {
		input_key_held(SDL_SCANCODE_W) - input_key_held(SDL_SCANCODE_S),
		input_key_held(SDL_SCANCODE_A) - input_key_held(SDL_SCANCODE_D)
	};

	// Normalize the input vector to handle diagonal movement properly
	if (input[0] != 0.0f || input[1] != 0.0f) {
		glm_vec2_normalize(input);
	}

	vec3 right_vector;
	glm_cross(camera->front, camera->up, right_vector);
	glm_normalize(right_vector);

	vec3 forward_vector = {camera->front[0], 0.0f, camera->front[2]}; // Zero out y-component
	glm_normalize(forward_vector); // Normalize the forward vector for consistent speed

	vec3 forward_movement = {0};
	vec3 right_movement = {0};

	controller->velocity[0] = 0.0f;
	controller->velocity[2] = 0.0f;
	// vec3 velocity = {0.0f, 0.0f, 0.0f};

	glm_vec3_scale(forward_vector, input[0] * speed, forward_movement);
	glm_vec3_add(controller->velocity, forward_movement, controller->velocity);

	glm_vec3_scale(right_vector, -input[1] * speed, right_movement);
	glm_vec3_add(controller->velocity, right_movement, controller->velocity);

	if (input_key_pressed(SDL_SCANCODE_SPACE) && controller->grounded) {
		controller->velocity[1] = controller->jump_height;
		controller->grounded = false;
		// audio_play_sound(res_pack, SOUND_JUMP);
	}
	
	controller->velocity[1] += gravity;

	vec3 coming_position = {0};
	coming_position[0] = transform->position[0] + controller->velocity[0];
	coming_position[1] = transform->position[1];
	coming_position[2] = transform->position[2] + controller->velocity[2];

	collision_t *collisions = temp_calloc(8 * sizeof(collision_t));
	i32 count = get_player_collisions(res_pack, grid, coming_position, &collider->box, collisions);
	collision_t best_collision = collisions[0];

	if (count > 0) {
		float max_y_difference = collisions[0].global_box.max_y - (coming_position[1] + collider->box.min_y);
		for (i32 i = 1; i < count; i++) {
			float y_difference = collisions[i].global_box.max_y - (coming_position[1] + collider->box.min_y);
			if (y_difference > max_y_difference) {
				max_y_difference = y_difference;
				best_collision = collisions[i];
			}
		}
	}

	if (best_collision.hit) {
		float y_difference = best_collision.global_box.max_y - (coming_position[1] + collider->box.min_y);
		if (controller->grounded && y_difference > 0.0f && y_difference < 0.2f) {
			transform->position[1] = best_collision.global_box.max_y - collider->box.min_y + 0.001f;
		}
	}
	
	transform->position[0] += controller->velocity[0];
	collision_t collision = get_first_player_collision(res_pack, grid, transform->position, &collider->box);
	if (collision.hit) {
		if (controller->velocity[0] < 0.0f) {
			transform->position[0] = collision.global_box.max_x - collider->box.min_x + 0.001f;
		}
		if (controller->velocity[0] > 0.0f) {
			transform->position[0] = collision.global_box.min_x - collider->box.max_x - 0.001f;
		}
	}
	
	transform->position[2] += controller->velocity[2];
	collision = get_first_player_collision(res_pack, grid, transform->position, &collider->box);
	if (collision.hit) {
		if (controller->velocity[2] < 0.0f) {
			transform->position[2] = collision.global_box.max_z - collider->box.min_z + 0.001f;
		}
		if (controller->velocity[2] > 0.0f) {
			transform->position[2] = collision.global_box.min_z - collider->box.max_z - 0.001f;
		}
	}

	transform->position[1] += controller->velocity[1];
	collision = get_first_player_collision(res_pack, grid, transform->position, &collider->box);
	if (collision.hit) {
		if (controller->velocity[1] > 0.0f) {
			transform->position[1] = collision.global_box.min_y - collider->box.max_y - 0.001f;
			controller->grounded = true;
			controller->velocity[1] = 0.0f;
		}
		if (controller->velocity[1] < 0.0f) {
			transform->position[1] = collision.global_box.max_y - collider->box.min_y + 0.001f;
			controller->grounded = true;
			controller->velocity[1] = 0.0f;
		}
	}
}

void game_update() {
	entity_t *player_ent = ent_get(state.player_ent_index);

    player_controller(&state.res_pack, &state.grid, player_ent);

    for (size_t i = 0; i < 1024; i++) {
		entity_t *ent = ent_get(i);
        if (ent->is_valid) {

        }
    }
}

static int compare_sprites(const void *a, const void *b) {
	entity_t *entity1 = *(entity_t **)a;
	entity_t *entity2 = *(entity_t **)b;

	entity_t *player_ent = ent_get(state.player_ent_index);

	vec3 *player_pos = player_ent->transform.position;
	
    vec3 *pos1 = entity1->transform.position;
	vec3 *pos2 = entity2->transform.position;

	float distance1 = glm_vec3_distance2(pos1, player_pos);
	float distance2 = glm_vec3_distance2(pos2, player_pos);

	if (distance1 < distance2) {
		return 1;
	} else if (distance1 > distance2) {
		return -1;
	} else {
		return 0;
	}
}

static size_t get_sorted_sprite_entities(entity_t **sorted_sprites) {
    size_t sorted_sprites_count = 0;

    for (size_t i = 0; i < 1024; i++) {
		entity_t *ent = ent_get(i);
        if (ent->is_valid && (ent->flags & HAS_SPRITE)) {
            // The entity has a sprite
            sorted_sprites[sorted_sprites_count] = ent;
            sorted_sprites_count++;
        }
    }

    qsort(sorted_sprites, sorted_sprites_count, sizeof(entity_t *), compare_sprites);

    return sorted_sprites_count;
}

void game_render() {
	entity_t *player_ent = ent_get(state.player_ent_index);

    render_game(&state.res_pack, &state.grid, player_ent->transform.position, &player_ent->camera);
    
	entity_t **sorted_sprites = temp_alloc(1024 * sizeof(entity_t *));
    size_t sorted_sprites_count = get_sorted_sprite_entities(sorted_sprites);
    
    for (size_t i = 0; i < sorted_sprites_count; i++) {
        render_sprite_transform(&sorted_sprites[i]->transform, &player_ent->camera, &sorted_sprites[i]->sprite);
    }

	// Crosshair
	render_image(&state.res_pack, TEX_CROSSHAIR, state.res_pack.render_width / 2 - 4, state.res_pack.render_height / 2 - 4, COLOR_WHITE);

	// Inventory
	for (i32 i = 0; i < 20; i++) {
		gui_button(&state.res_pack, "", (rect_t){i * 2, 43, 2, 2});
	}
}

void game_input(SDL_Event event, entity_t *player_ent) {
	if (event.type == SDL_MOUSEMOTION) {
		if (state.cursor_free) {
			return;
		}

		float x_offset = (float)(event.motion.xrel);
		float y_offset = -(float)(event.motion.yrel);
		
		float sensitivity = 0.1f;
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		camera_c *camera = &player_ent->camera;

		camera->yaw += x_offset;
		camera->pitch += y_offset;

		if (camera->pitch > 89.0f)
			camera->pitch = 89.0f;
		if (camera->pitch < -89.0f)
			camera->pitch = -89.0f;

		vec3 front = {0};
		front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
		front[1] = sinf(glm_rad(camera->pitch));
		front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));

		glm_vec3_normalize(front);
		glm_vec3_copy(front, camera->front);
	}
}