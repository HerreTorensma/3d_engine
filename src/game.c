#include "entity.h"
#include "engine/arena.h"
#include "engine/render.h"
#include "engine/res.h"
#include "engine/core.h"
#include "engine/collision.h"

#include "load_game.c"
#include "prefabs.c"

enum {
    STATE_MAIN_MENU,
    STATE_GAMEPLAY,
};

typedef struct state {
    // entity_t entities[1024];
    index_t player_ent_index;
    res_pack_t res_pack;
    grid_t grid;
    index_t mode;
	SDL_Window *window;
	bool cursor_free;
	ent_system_t ent_system;
	// arena_t temp_arena;

	bool edit_mode;
} state_t;

static state_t state = {0};

void game_init() {
	window_width = 1280;
	window_height = 720;

	state.window = create_sdl2_window("Idk game", window_width, window_height);
	SDL_GLContext *context = create_sdl2_gl_context(state.window, window_width, window_height);

	// arena_init(&state.temp_arena, 1024 * 1024);

    load_game(&state.res_pack);

    state.mode = STATE_GAMEPLAY;

    font_init(&state.res_pack.font, &state.res_pack, TEX_FONT);
	state.res_pack.font.y_center = -4;

	// grid_init(&grid, 512, 16, 512);
	// grid_load(&grid, "test.grid");
	grid_load(&state.grid, "test2.grid");

    state.player_ent_index = spawn_player((transform_t){0});

	spawn_barrel((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 5.0f});
	spawn_tree((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 6.0f});

    render_init(&state.res_pack);
	editor_init();
}

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

	// collision_t *collisions = arena_calloc(&state.temp_arena, 8 * sizeof(collision_t));
	collision_t *collisions = arena_calloc(&temp_arena, 8 * sizeof(collision_t));
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

static void compare_sprites(const void *a, const void *b) {
    entity_t *entity1 = (entity_t *)a;
	entity_t *entity2 = (entity_t *)b;

	entity_t *player_ent = ent_get(state.player_ent_index);

	vec3 *player_pos = player_ent->transform.position;
	
    vec3 *pos1 = entity1->transform.position;
	vec3 *pos2 = entity2->transform.position;

	float distance1 = ((*player_pos[0] - *pos1[0]) * (*player_pos[0] - *pos1[0])) + ((*player_pos[1] - *pos1[1]) * (*player_pos[1] - *pos1[1])) + ((*player_pos[2] - *pos1[2]) * (*player_pos[2] - *pos1[2]));
	float distance2 = ((*player_pos[0] - *pos2[0]) * (*player_pos[0] - *pos2[0])) + ((*player_pos[1] - *pos2[1]) * (*player_pos[1] - *pos2[1])) + ((*player_pos[2] - *pos2[2]) * (*player_pos[2] - *pos2[2]));

	if (distance1 > distance2) {
		return -1;
	} else if (distance1 < distance2) {
		return 1;
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
            sorted_sprites[sorted_sprites_count] = &ent->sprite;
            sorted_sprites_count++;
        }
    }

    qsort(sorted_sprites, sorted_sprites_count, sizeof(entity_t *), compare_sprites);

    return sorted_sprites_count;
}

void game_render(res_pack_t *res_pack) {
	entity_t *player_ent = ent_get(state.player_ent_index);

    render_game(&state.res_pack, &state.grid, player_ent->transform.position, &player_ent->camera);

    // entity_t **sorted_sprites = arena_alloc(&state.temp_arena, 1024 * sizeof(entity_t *));
    entity_t **sorted_sprites = arena_alloc(&temp_arena, 1024 * sizeof(entity_t *));
    size_t sorted_sprites_count = get_sorted_sprite_entities(sorted_sprites);
    
    for (size_t i = 0; i < 1024; i++) {
        render_sprite_transform(&sorted_sprites[i]->transform, &player_ent->camera, &sorted_sprites[i]->sprite);
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