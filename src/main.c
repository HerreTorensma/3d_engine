#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"
#include "engine/ecs.h"
#include "engine/gui.h"
#include "engine/grid.h"

static vertex_t quad_vertices[] = {
    {{1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}},  // top right
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // bottom left
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}}   // top left 
};
static u32 quad_indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

typedef struct {
    vec3 speed;
} rotating_c;

enum {
	MESH_CUBE = 2,
	MESH_SLAB,
	MESH_CORNER,
	MESH_FLOOR,
	MESH_PYRAMID,
	MESH_SLOPE,
	MESH_MONKEY,
	MESH_MUSHROOM,
};

enum {
	TEX_BRICKS = 1,
	TEX_DIRT,
	TEX_COBBLE,
	TEX_IRON_BARS,
	TEX_TREE,
	TEX_BIRCH,
	TEX_CROSSHAIR,
	TEX_WORMFISH,
	TEX_FONT,
	TEX_BUTTON,
	TEX_BUTTON_PRESSED,
	TEX_FRAME,
	TEX_GRASS,
};

// enum {
// 	TEX_BRICKS = 1,
// 	TEX_IRON_BARS,
// 	TEX_DIRT,
// 	TEX_COBBLE,
// 	TEX_TREE,
// 	TEX_BIRCH,
// 	TEX_CROSSHAIR,
// 	TEX_WORMFISH,
// 	TEX_FONT,
// 	TEX_BUTTON,
// 	TEX_BUTTON_PRESSED
// };

static camera_t create_camera() {
	camera_t camera = {0};

	glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera.position);
	// glm_vec3_copy((vec3){0.0f, 0.3f, 3.0f}, camera.position);
	// glm_vec3_copy((vec3){0.0f, 1.0f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);

	camera.pitch = 0.0f;
	camera.yaw = -90.0f;

	return camera;
}

static camera_t camera = {0};

void game_input(SDL_Event event) {
	if (event.type == SDL_MOUSEMOTION) {
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

const int ROTATING_C = 3;

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

void game_update(ecs_world_t *ecs) {
	float camera_speed = 0.05f;
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

	vec3 right_vector;
	glm_cross(camera.front, camera.up, right_vector);
	glm_normalize(right_vector);

	// Calculate the forward vector based on horizontal components
	vec3 forward_vector = { camera.front[0], 0.0f, camera.front[2] }; // Zero out y-component
	glm_normalize(forward_vector); // Normalize the forward vector for consistent speed

	if (input_key_held(SDL_SCANCODE_W)) {
		vec3 temp2;
		glm_vec3_scale(forward_vector, camera_speed, temp2); // Move forward
		glm_vec3_add(camera.position, temp2, camera.position); // Update position
	}

	if (input_key_held(SDL_SCANCODE_S)) {
		vec3 temp2;
		glm_vec3_scale(forward_vector, -camera_speed, temp2); // Move backward
		glm_vec3_add(camera.position, temp2, camera.position); // Update position
	}

	if (input_key_held(SDL_SCANCODE_A)) {
		vec3 temp;
		glm_cross(camera.front, camera.up, temp);
		glm_normalize(temp);

		vec3 temp2;
		glm_vec3_scale(temp, -camera_speed, temp2);
		glm_vec3_add(camera.position, temp2, camera.position);
	}
	if (input_key_held(SDL_SCANCODE_D)) {
		vec3 temp;
		glm_cross(camera.front, camera.up, temp);
		glm_normalize(temp);

		vec3 temp2;
		glm_vec3_scale(temp, camera_speed, temp2);
		glm_vec3_add(camera.position, temp2, camera.position);
	}

	rotating_system(ecs);
}

#define FPS 120

int main(int argc, char *argv[]) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_EVENTS) < 0) {
		debug_log("Failed to initialize SDL2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	window_width = 1280;
	window_height = 720;

	// Create the window
	SDL_Window *window = SDL_CreateWindow(
		"game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!window) {
		debug_log("Failed to create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Make an OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
		debug_log("Failed to create OpenGL context: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		debug_log("Failed to load OpenGL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	debug_log("OpenGL loaded\n");
	debug_log("Vendor: %s\n", glGetString(GL_VENDOR));
	debug_log("Renderer: %s\n", glGetString(GL_RENDERER));
	debug_log("Version: %s\n", glGetString(GL_VERSION));
	
	glViewport(0, 0, window_width, window_height);

	res_pack_t res_pack = {0};
	
	res_pack.render_width = 640;
	res_pack.render_height = 360;

	// res_pack.render_width = 800;
	// res_pack.render_height = 450;

	// res_pack.render_height = 480;
	res_pack.sky_color = (color_t){58, 49, 41, 255};
	// res_pack.sky_color = (color_t){2, 9, 23, 255};
	// res_pack.sky_color = (color_t){0, 0, 0, 255};
	res_pack.fog_color = (color_t){58, 49, 41, 255};
	res_pack.editor_color = (color_t){50, 50, 50, 255};

	// render_init(&res_pack);

	res_add_texture(&res_pack, TEX_BRICKS, load_tga("res/images/bricks.tga", true));
	res_add_texture(&res_pack, TEX_DIRT, load_tga("res/images/dirt.tga", true));
	res_add_texture(&res_pack, TEX_COBBLE, load_tga("res/images/cobble.tga", false));
	res_add_texture(&res_pack, TEX_IRON_BARS, load_tga("res/images/iron_bars.tga", true));
	res_add_texture(&res_pack, TEX_TREE, load_tga("res/images/tree.tga", true));
	res_add_texture(&res_pack, TEX_BIRCH, load_tga("res/images/birch.tga", true));
	res_add_texture(&res_pack, TEX_CROSSHAIR, load_tga("res/images/crosshair.tga", true));
	res_add_texture(&res_pack, TEX_WORMFISH, load_tga("res/images/wormfish.tga", true));
	res_add_texture(&res_pack, TEX_FONT, load_tga("res/images/font.tga", false));

	res_add_texture(&res_pack, TEX_BUTTON, load_tga("res/images/button_sheet.tga", false));
	res_add_texture(&res_pack, TEX_BUTTON_PRESSED, load_tga("res/images/button_pressed_sheet.tga", false));
	res_pack.button_tex_index = TEX_BUTTON;
	res_pack.button_pressed_tex_index = TEX_BUTTON_PRESSED;
	res_pack.gui_tile_size = 8;

	res_add_texture(&res_pack, TEX_FRAME, load_tga("res/images/frame_new.tga", false));
	res_add_texture(&res_pack, TEX_GRASS, load_tga("res/images/grass.tga", false));

	res_add_mesh_raw(&res_pack, MESH_QUAD, quad_vertices, sizeof(quad_vertices) / sizeof(vertex_t), quad_indices, sizeof(quad_indices) / sizeof(u32));
	res_add_mesh(&res_pack, MESH_CUBE, load_mesh("res/meshes/cube.mesh"));
	res_add_mesh(&res_pack, MESH_FLOOR, load_mesh("res/meshes/floor.mesh"));
	res_add_mesh(&res_pack, MESH_SLAB, load_mesh("res/meshes/slab.mesh"));
	res_add_mesh(&res_pack, MESH_SLOPE, load_mesh("res/meshes/slope.mesh"));
	res_add_mesh(&res_pack, MESH_PYRAMID, load_mesh("res/meshes/pyramid.mesh"));
	res_add_mesh(&res_pack, MESH_CORNER, load_mesh("res/meshes/corner.mesh"));
	res_add_mesh(&res_pack, MESH_MONKEY, load_mesh("res/meshes/monkey.mesh"));
	res_add_mesh(&res_pack, MESH_MUSHROOM, load_mesh("res/meshes/mushroom.mesh"));

	// font_t font = {0};
	font_init(&res_pack.font, &res_pack, TEX_FONT);
	res_pack.font.y_center = -4;
	res_pack.font.color = COLOR_BLACK;

	grid_t grid = {0};
	// grid_init(&grid, 512, 16, 512);
	grid_load(&grid, "test.grid");

	// grid_init(&grid, 64, 64, 64);
	// grid_init(&grid, 16, 16, 16);

	// for (int z = 0; z < 16; z++) {
	// 	for (int x = 0; x < 16; x++) {
	// 		grid_set_cell(&grid, TILE_DIRT_FLOOR, x, 0, z);
	// 	}
	// }

	// grid_set_cell(&grid, TILE_BRICK_CUBE, 0, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 1, 0, 0);
	// // grid_set_cell(&grid, TILE_BRICK_CUBE, 2, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 3, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 4, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 5, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 6, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 7, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 8, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 0);

	// grid_set_cell(&grid, TILE_BRICK_CUBE, 7, 1, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 8, 1, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 1, 0);

	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 1);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 2);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 3);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 4);
	
	// grid_set_cell(&grid, TILE_BRICK_SLOPE, 10, 0, 4);
	// grid_set_cell(&grid, TILE_BRICK_FLOOR, 11, 0, 4);
	// grid_set_cell(&grid, TILE_BRICK_PYRAMID, 12, 0, 4);
	// grid_set_cell(&grid, TILE_BRICK_SLAB, 13, 0, 4);
	// grid_set_cell(&grid, TILE_BRICK_CORNER, 14, 0, 4);
	// grid_set_cell(&grid, TILE_BRICK_MONKEY, 15, 0, 4);

	camera = create_camera();

	bool edit_mode = false;
	bool fullscreen = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Don't lock fps
	// SDL_GL_SetSwapInterval(0);

	// ecs_world_t ecs = ecs_create();
	ecs_world_t ecs = {0};
	ecs_init(&ecs);
	ECS_REGISTER(&ecs, transform_c, TRANSFORM_C);
	ECS_REGISTER(&ecs, sprite_c, SPRITE_C);
	ECS_REGISTER(&ecs, mesh_c, MESH_C);
	ECS_REGISTER(&ecs, rotating_c, ROTATING_C);

	for (int i = 0; i < 100; i++) {
		entity_t tree_e = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, tree_e, TRANSFORM_C);
		transform->position[0] = 10.0f;
		transform->position[1] = 0.0f;
		transform->position[2] = 5.0f + (float)i;

		// ECS_SET(&ecs, tree, sprite_c, {TEX_BIRCH, true, 1.0f, 2.0f});
		sprite_c *sprite = ecs_set(&ecs, tree_e, SPRITE_C);
		sprite->texture_index = TEX_BIRCH;
		sprite->billboard = true;
		sprite->x_scale = 1.0f;
		sprite->y_scale = 2.0f;
	}

	{
		entity_t worm_e = ecs_new(&ecs);
		transform_c *transform = ecs_set(&ecs, worm_e, TRANSFORM_C);
		transform->position[0] = 5.0f;
		transform->position[1] = 0.0f;
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
		transform->position[1] = 0.0f;
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

		transform->position[0] = 1.0f;
		transform->position[1] = 1.0f;
		transform->position[2] = 1.0f;

		transform->rotation[1] = 45.0f;

		mesh->mesh_index = MESH_MONKEY;
		mesh->texture_index = TEX_BRICKS;
	}

	render_init(&res_pack);
	editor_init();

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
			editor_update(&grid);
		} else {
			game_update(&ecs);
		}

		if (edit_mode) {
			// render_start_frame_buffer(&res_pack);

			editor_render(&res_pack, &grid);

			// render_end_frame_buffer(&res_pack);
		} else {
			render_start_frame_buffer(&res_pack);

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

			gui_print(&res_pack, &res_pack.font, "DREAM SIMULATOR v0.1", 1, 0);


			render_end_frame_buffer(&res_pack);
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