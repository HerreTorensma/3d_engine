#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"
#include "engine/ecs.h"
#include "engine/gui.h"
#include "engine/grid.h"
#include "engine/audio.h"
#include "engine/arena.h"
#include "engine/collision.h"

typedef struct {
    vec3 speed;
} rotating_c;

// Generic flags
typedef enum entity_flag {
	IS_PLAYER,
	IS_SPRITE,
	IS_BILLBOARD_SPRITE,
	IS_MESH,
} entity_flag_t;

// Specific types
// typedef enum entity_type {

// } entity_type_t;

typedef struct entity {
	transform_t transform;

	camera_t camera;

	index_t texture_index;

	index_t mesh_index;
} entity;

enum {
	ROTATING_C = 4,
};

static camera_t create_camera() {
	camera_t camera = {0};

	// glm_vec3_copy((vec3){3.0f, 0.5f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){3.0f, 1.5f, 3.0f}, camera.position);

	// glm_vec3_copy((vec3){0.0f, 0.3f, 3.0f}, camera.position);
	// glm_vec3_copy((vec3){0.0f, 1.0f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);

	camera.pitch = 0.0f;
	camera.yaw = -90.0f;

	return camera;
}

static camera_t camera = {0};
static box_t player_box = {
	.min_x = -0.15f,
	.max_x = 0.15f,
	
	.min_y = -0.5f,
	.max_y = 0.0f,

	.min_z = -0.15f,
	.max_z = 0.15f,
};

static bool frozen = false;

void game_input(SDL_Event event) {
	if (event.type == SDL_MOUSEMOTION) {
		if (frozen) {
			return;
		}

		float x_offset = (float)(event.motion.xrel);
		float y_offset = -(float)(event.motion.yrel);
		
		float sensitivity = 0.1f;
		x_offset *= sensitivity;
		y_offset *= sensitivity;

		camera.yaw += x_offset;
		camera.pitch += y_offset;

		if (camera.pitch > 89.0f)
			camera.pitch = 89.0f;
		if (camera.pitch < -89.0f)
			camera.pitch = -89.0f;

		vec3 front = {0};
		front[0] = cosf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));
		front[1] = sinf(glm_rad(camera.pitch));
		front[2] = sinf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));

		glm_vec3_normalize(front);
		glm_vec3_copy(front, camera.front);
	}
}

static void rotating_system(ecs_world_t *ecs) {
	ecs_query_t query = ecs_query(ecs, TRANSFORM_C, ROTATING_C, -1);
	for (size_t i = 0; i < query.len; i++) {
		transform_c *transform = ecs_get(ecs, query.entities[i], TRANSFORM_C);
		rotating_c *rotating = ecs_get(ecs, query.entities[i], ROTATING_C);

		transform->rotation[0] += rotating->speed[0];
		transform->rotation[1] += rotating->speed[1];
		transform->rotation[2] += rotating->speed[2];
	}
}

bool global_colliding = false;

static bool player_grounded = false;
// static vec3 player_velocity = {0};
static vec3 velocity = {0};

#include "res_loading.c"

void game_update(res_pack_t *res_pack, grid_t *grid, ecs_world_t *ecs, SDL_Window *window) {
	if (input_key_pressed(SDL_SCANCODE_F)) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);
		frozen = true;
	}

	if (input_key_released(SDL_SCANCODE_F)) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		frozen = false;
	}

	float camera_speed = 0.05f;

	if (input_key_held(SDL_SCANCODE_LSHIFT)) {
		camera_speed = 0.005f;
	}

	if (input_key_pressed(SDL_SCANCODE_T)) {
		audio_play_sound_3d(res_pack, SOUND_JUMP, camera.position, camera.front, (vec3){10.0f, 1.0f, 10.0f});
	}

	// if (input_key_held(SDL_SCANCODE_W)) {
	// 	vec3 temp;
	// 	glm_vec3_scale(camera.front, camera_speed, temp);
	// 	glm_vec3_add(camera.position, temp, camera.position);
	// }
	// if (input_key_held(SDL_SCANCODE_S)) {
	// 	vec3 temp;
	// 	glm_vec3_scale(camera.front, -camera_speed, temp);
	// 	glm_vec3_add(camera.position, temp, camera.position);
	// }
	rotating_system(ecs);

	vec2 input = {
		input_key_held(SDL_SCANCODE_W) - input_key_held(SDL_SCANCODE_S),
		input_key_held(SDL_SCANCODE_A) - input_key_held(SDL_SCANCODE_D)
	};

	// Normalize the input vector to handle diagonal movement properly
	if (input[0] != 0.0f || input[1] != 0.0f) {
		glm_vec2_normalize(input);
	}

	// Calculate forward and right vectors
	vec3 right_vector;
	glm_cross(camera.front, camera.up, right_vector);
	glm_normalize(right_vector);

	vec3 forward_vector = {camera.front[0], 0.0f, camera.front[2]}; // Zero out y-component
	glm_normalize(forward_vector); // Normalize the forward vector for consistent speed

	// Calculate movement based on input vector
	vec3 forward_movement = {0.0f, 0.0f, 0.0f};
	vec3 right_movement = {0.0f, 0.0f, 0.0f};

	velocity[0] = 0.0f;
	velocity[2] = 0.0f;
	// vec3 velocity = {0.0f, 0.0f, 0.0f};

	// Scale forward movement by the input's vertical component (input[0])
	glm_vec3_scale(forward_vector, input[0] * camera_speed, forward_movement);
	glm_vec3_add(velocity, forward_movement, velocity);

	// Scale right movement by the input's horizontal component (input[1])
	glm_vec3_scale(right_vector, -input[1] * camera_speed, right_movement); // Note the negative to align direction
	glm_vec3_add(velocity, right_movement, velocity);

	// Update camera position
	// glm_vec3_add(camera.position, total_movement, camera.position);

	if (input_key_pressed(SDL_SCANCODE_SPACE) && player_grounded) {
		velocity[1] = 0.12f;
		audio_play_sound(res_pack, SOUND_JUMP);
	}
	velocity[1] -= 0.005f;

	camera.position[0] += velocity[0];
	collision_t collision = check_player_collision(res_pack, grid, &camera, &player_box);
	if (collision.hit) {
		if (velocity[0] > 0) {
			camera.position[0] = collision.global_box.min_x - player_box.max_x - 0.001f;
		}
		if (velocity[0] < 0) {
			camera.position[0] = collision.global_box.max_x - player_box.min_x + 0.001f;
		}
	}

	player_grounded = false;
	camera.position[1] += velocity[1];
	collision = check_player_collision(res_pack, grid, &camera, &player_box);
	if (collision.hit) {
		if (velocity[1] > 0) {
			camera.position[1] = collision.global_box.min_y - player_box.max_y - 0.001f;
			player_grounded = true;
			velocity[1] = 0.0f;
		}
		if (velocity[1] < 0) {
			camera.position[1] = collision.global_box.max_y - player_box.min_y + 0.001f;
			player_grounded = true;
			velocity[1] = 0.0f;
		}
	}

	camera.position[2] += velocity[2];
	collision = check_player_collision(res_pack, grid, &camera, &player_box);
	if (collision.hit) {
		if (velocity[2] > 0) {
			camera.position[2] = collision.global_box.min_z - player_box.max_z - 0.001f;
		}
		if (velocity[2] < 0) {
			camera.position[2] = collision.global_box.max_z - player_box.min_z + 0.001f;
		}
	}
}

#define FPS 120

SDL_Window *create_sdl2_window(i32 width, i32 height) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_EVENTS) < 0) {
		debug_log("Failed to initialize SDL2: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Initialize audio
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("Failed to open audio\n");
		return EXIT_FAILURE;
	}

	// Create the window
	SDL_Window *window = SDL_CreateWindow(
		"game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!window) {
		debug_log("Failed to create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	return window;
}

SDL_GLContext *create_sdl2_gl_context(SDL_Window *window, i32 width, i32 height) {
	// Make an OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
		debug_log("Failed to create OpenGL context: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		debug_log("Failed to load OpenGL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	debug_log("OpenGL loaded\n");
	debug_log("Vendor: %s\n", glGetString(GL_VENDOR));
	debug_log("Renderer: %s\n", glGetString(GL_RENDERER));
	debug_log("Version: %s\n", glGetString(GL_VERSION));
	
	glViewport(0, 0, width, height);

	return context;
}

int main(int argc, char *argv[]) {
	// init_sdl2();
	window_width = 1280;
	window_height = 720;

	SDL_Window *window = create_sdl2_window(window_width, window_height);
	SDL_GLContext *context = create_sdl2_gl_context(window, window_width, window_height);

	res_pack_t res_pack = {0};

	load_res(&res_pack);

	font_init(&res_pack.font, &res_pack, TEX_FONT);
	res_pack.font.y_center = -4;

	grid_t grid = {0};
	// grid_init(&grid, 512, 16, 512);
	// grid_load(&grid, "test.grid");
	grid_load(&grid, "test2.grid");

	camera = create_camera();

	bool edit_mode = false;
	bool fullscreen = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Don't lock fps
	// SDL_GL_SetSwapInterval(0);

	ecs_world_t ecs = {0};
	ecs_init(&ecs);
	ECS_REGISTER(&ecs, transform_c, TRANSFORM_C);
	ECS_REGISTER(&ecs, sprite_c, SPRITE_C);
	ECS_REGISTER(&ecs, mesh_c, MESH_C);
	ECS_REGISTER(&ecs, rotating_c, ROTATING_C);
	ECS_REGISTER(&ecs, collider_c, COLLIDER_C);

	for (int i = 0; i < 100; i++) {
		entity_t tree_e = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, tree_e, TRANSFORM_C);
		transform->position[0] = 10.0f;
		transform->position[1] = 1.0f;
		transform->position[2] = 5.0f + (float)i;

		// ECS_SET(&ecs, tree, sprite_c, {TEX_BIRCH, true, 1.0f, 2.0f});
		sprite_c *sprite = ecs_set(&ecs, tree_e, SPRITE_C);
		sprite->texture_index = TEX_BIRCH;
		sprite->billboard = true;
		sprite->x_scale = 1.0f;
		sprite->y_scale = 1.0f;
	}

	{
		entity_t worm_e = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, worm_e, TRANSFORM_C);
		transform->position[0] = 5.0f;
		transform->position[1] = 1.0f;
		transform->position[2] = 5.0f;

		// ECS_SET(&ecs, worm_e, sprite_c, {TEX_WORMFISH, true, 2.0f, 1.0f});
		sprite_c *sprite = ecs_set(&ecs, worm_e, SPRITE_C);
		sprite->texture_index = TEX_WORMFISH;
		sprite->billboard = true;
		sprite->x_scale = 2.0f;
		sprite->y_scale = 1.0f;
	}

	// Iron bars
	{
		entity_t bars = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, bars, TRANSFORM_C);
		transform->position[0] = 2.0f;
		transform->position[1] = 1.0f;
		transform->position[2] = 0.0f;

		sprite_c *sprite = ecs_set(&ecs, bars, SPRITE_C);
		sprite->texture_index = TEX_IRON_BARS;
		sprite->billboard = false;
		sprite->x_scale = 1.0f;
		sprite->y_scale = 1.0f;
	}

	{
		entity_t monke = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, monke, TRANSFORM_C);
		mesh_c *mesh = ecs_set(&ecs, monke, MESH_C);
		rotating_c *rotating = ecs_set(&ecs, monke, ROTATING_C);

		rotating->speed[0] = 1.0f;
		// rotating->speed[1] = 1.0f;
		// rotating->speed[2] = 1.0f;

		transform->position[0] = 10.0f;
		transform->position[1] = 2.0f;
		transform->position[2] = 10.0f;

		transform->rotation[1] = 45.0f;

		mesh->mesh_index = MESH_MONKEY;
		mesh->texture_index = TEX_BRICKS;
	}

	render_init(&res_pack);
	editor_init();

	arena_t temp_arena = {0};
	// 1 MB
	arena_init(&temp_arena, 1024*1024);

	// Window loop
	bool running = true;
	SDL_Event event;
	while (running) {
		// Quit if there is a quit event
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				i32 w, h;
				SDL_GetWindowSize(window, &w, &h);
				glViewport(0, 0, w, h);
				window_width = w;
				window_height = h;
			}

			if (edit_mode) {
				editor_input(event);
			} else {
				game_input(event);
			}
		}

		arena_clear(&temp_arena);

		input_update();

		if (input_key_pressed(SDL_SCANCODE_F11)) {
			fullscreen = !fullscreen;
			if (fullscreen) {
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			} else {
				SDL_SetWindowFullscreen(window, 0);
			}
		}

		if (input_key_pressed(SDL_SCANCODE_E)) {
			edit_mode = !edit_mode;
			if (edit_mode) {
				SDL_SetRelativeMouseMode(SDL_FALSE);
			} else {
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}

		if (edit_mode) {
			editor_update(&res_pack, &grid);
		} else {
			game_update(&res_pack, &grid, &ecs, window);
		}

		if (edit_mode) {
			// render_start_frame_buffer(&res_pack);

			// editor_render(&res_pack, &grid, MESH_EDITOR_STOP, TEX_EDITOR_STOP);
			editor_render(&res_pack, &grid, 12, 6);

			// render_end_frame_buffer(&res_pack);
		} else {
			// render_start_frame_buffer(&res_pack);

			render_game(&res_pack, &grid, &ecs, &camera);

			// Crosshair
			render_image(&res_pack, TEX_CROSSHAIR, res_pack.render_width / 2 - 4, res_pack.render_height / 2 - 4, COLOR_WHITE);

			// render_image(res_pack, PIVOT_TOP_LEFT, 6, 1, 1);
			render_image(&res_pack, TEX_TREE, 0, 0, COLOR_WHITE);
			// render_image(&res_pack, TEX_BIRCH, 0, -4, COLOR_WHITE);
			render_image(&res_pack, TEX_TREE, 640-64, 0, COLOR_WHITE);
			// render_image(&res_pack, TEX_BIRCH, 640-64, -4, COLOR_WHITE);

			render_image(&res_pack, TEX_FRAME, 0, 0, COLOR_WHITE);
			// render_mesh_isometric(&res_pack, res_pack.meshes[MESH_CUBE], 1, 100, 100, 32.0f);
			// render_mesh_isometric(&res_pack, res_pack.meshes[MESH_SLAB], 2, 200, 100, 32.0f);

			// render_image_rect(&res_pack, TEX_COBBLE, (rect_t){0, 0, 16, 16}, (rect_t){16, 16, 128, 128}, COLOR_WHITE);

			gui_button(&res_pack, "", (rect_t){40, 1, 2, 2});
			gui_button(&res_pack, "", (rect_t){43, 1, 2, 2});
			gui_button(&res_pack, "", (rect_t){46, 1, 2, 2});
			gui_button(&res_pack, "", (rect_t){49, 1, 2, 2});
			gui_button(&res_pack, "", (rect_t){52, 1, 2, 2});

			#ifdef DEBUG
			gui_print(&res_pack, &res_pack.font, "DREAM SIMULATOR v0.1", 1, 0, COLOR_WHITE);
			{
				// char buffer[64];
				char *buffer = arena_alloc(&temp_arena, 64);
				sprintf(buffer, "X: %f Y: %f Z: %f", camera.position[0], camera.position[1], camera.position[2]);
				gui_print(&res_pack, &res_pack.font, buffer, 1, 8, COLOR_WHITE);
			}
			{
				// char buffer[64];
				char *buffer = arena_alloc(&temp_arena, 64);
				i32 map_x = (i32)roundf(camera.position[0]);
				i32 map_y = (i32)roundf(camera.position[1]);
				i32 map_z = (i32)roundf(camera.position[2]);

				tile_t current_tile = grid_get_cell(&grid, map_x, map_y, map_z);
				sprintf(buffer, "MESH: %d", current_tile.mesh_index);
				gui_print(&res_pack, &res_pack.font, buffer, 1, 16, COLOR_WHITE);
			}

			// {
			// 	// char buffer[64];
			// 	char *buffer = arena_alloc(&temp_arena, 64);
			// 	sprintf(buffer, "X: %f Y: %f Z: %f", camera.front[0], camera.front[1], camera.front[2]);
			// 	gui_print(&res_pack, &res_pack.font, buffer, 1, 24, COLOR_WHITE);
			// }

			if (global_colliding) {
				gui_print(&res_pack, &res_pack.font, "COLLIDING", 1, 24, COLOR_WHITE);
			}
			#endif

			// render_end_frame_buffer(&res_pack);
		}

		SDL_GL_SwapWindow(window);

		// SDL_Delay(1000/FPS);
	}

	debug_log("Quitting\n");

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}