#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"
#include "engine/gui.h"
#include "engine/grid.h"
#include "engine/audio.h"
#include "engine/arena.h"
#include "engine/collision.h"
#include "entity.h"

#include "game.c"

struct engine_state {
	bool fullscreen;
};

// static bool frozen = false;

// void game_input(SDL_Event event, entity_t *player_ent) {
// 	if (event.type == SDL_MOUSEMOTION) {
// 		if (frozen) {
// 			return;
// 		}

// 		float x_offset = (float)(event.motion.xrel);
// 		float y_offset = -(float)(event.motion.yrel);
		
// 		float sensitivity = 0.1f;
// 		x_offset *= sensitivity;
// 		y_offset *= sensitivity;

// 		camera_c *camera = &player_ent->camera;

// 		camera->yaw += x_offset;
// 		camera->pitch += y_offset;

// 		if (camera->pitch > 89.0f)
// 			camera->pitch = 89.0f;
// 		if (camera->pitch < -89.0f)
// 			camera->pitch = -89.0f;

// 		vec3 front = {0};
// 		front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
// 		front[1] = sinf(glm_rad(camera->pitch));
// 		front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));

// 		glm_vec3_normalize(front);
// 		glm_vec3_copy(front, camera->front);
// 	}
// }

// void game_update(res_pack_t *res_pack, grid_t *grid, SDL_Window *window, entity_t *player_entnt) {
// 	if (input_key_pressed(SDL_SCANCODE_F)) {
// 		SDL_SetRelativeMouseMode(SDL_FALSE);
// 		SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);
// 		frozen = true;
// 	}

// 	if (input_key_released(SDL_SCANCODE_F)) {
// 		SDL_SetRelativeMouseMode(SDL_TRUE);
// 		frozen = false;
// 	}

// 	player_controller_system(res_pack, grid, window, player_entnt);

// 	// rotating_system(ecs);
// }

#define FPS 120

SDL_Window *create_sdl2_window(const char title, i32 width, i32 height) {
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
		title,
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
	// window_width = 1280;
	// window_height = 720;

	SDL_Window *window = create_sdl2_window("Fuckass game", window_width, window_height);
	SDL_GLContext *context = create_sdl2_gl_context(window, window_width, window_height);

	// res_pack_t res_pack = {0};

	// load_res(&res_pack);

	// resize_window(&res_pack, window);

	// font_init(&res_pack.font, &res_pack, TEX_FONT);
	// res_pack.font.y_center = -4;

	// grid_t grid = {0};
	// // grid_init(&grid, 512, 16, 512);
	// // grid_load(&grid, "test.grid");
	// grid_load(&grid, "test2.grid");

	// bool edit_mode = false;
	// bool fullscreen = false;

	// SDL_SetRelativeMouseMode(SDL_TRUE);

	// Don't lock fps
	// SDL_GL_SetSwapInterval(0);

	// vec3_t thing = (vec3_t){1, 2, 3};

	// printf("thing: %f %f %f", thing.x, thing.y, thing.z);
	// printf("thing: %f %f %f", thing.data[0], thing.data[1], thing.data[2]);

	// entity_t *player_ent = spawn_player((transform_t){0});

	// spawn_barrel((transform_t){.position = (vec3){5.0f, 1.0f, 5.0f}});
	// spawn_barrel((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 5.0f});
	
	// spawn_tree((transform_t){.position = (vec3){5.0f, 1.0f, 6.0f}});
	// spawn_tree((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 6.0f});

	// render_init(&res_pack);
	// editor_init();

	// 1 MB
	arena_init(&temp_arena, 1024*1024);

	game_init();

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
				resize_window(window);
			}

			if (edit_mode) {
				editor_input(event);
			} else {
				game_input(event, player_ent);
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
			// game_update(&res_pack, &grid, &ecs, window, player_ent);
			game_update(&res_pack, &grid, window, player_ent);
		}

		if (edit_mode) {
			// render_start_frame_buffer(&res_pack);

			// editor_render(&res_pack, &grid, MESH_EDITOR_STOP, TEX_EDITOR_STOP);
			editor_render(&res_pack, &grid, 13, 13);

			// render_end_frame_buffer(&res_pack);
		} else {
			render_game(&res_pack, &grid, player_ent->transform.position, &player_ent->camera);

			// Crosshair
			render_image(&res_pack, TEX_CROSSHAIR, res_pack.render_width / 2 - 4, res_pack.render_height / 2 - 4, COLOR_WHITE);

            // Inventory
			for (i32 i = 0; i < 20; i++) {
				gui_button(&res_pack, "", (rect_t){i * 2, 43, 2, 2});
			}

			#ifdef DEBUG
			gui_print(&res_pack, &res_pack.font, "DREAM SIMULATOR v0.1", 1, 0, COLOR_WHITE);
			{
				// char buffer[64];
				char *buffer = arena_alloc(&temp_arena, 64);
				sprintf(buffer, "X: %f Y: %f Z: %f", player_ent->transform.position[0], player_ent->transform.position[1], player_ent->transform.position[2]);
				gui_print(&res_pack, &res_pack.font, buffer, 1, 8, COLOR_WHITE);
			}
			{
				// char buffer[64];
				char *buffer = arena_alloc(&temp_arena, 64);
				i32 map_x = (i32)roundf(player_ent->transform.position[0]);
				i32 map_y = (i32)roundf(player_ent->transform.position[1]);
				i32 map_z = (i32)roundf(player_ent->transform.position[2]);

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
		}

		SDL_GL_SwapWindow(window);
	}

	debug_log("Quitting\n");

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}