#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"
#include "engine/ecs.h"
#include "engine/gui.h"

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
	TEX_IRON_BARS,
	TEX_TREE,
	TEX_BIRCH,
	TEX_DIRT,
	TEX_CROSSHAIR,
	TEX_WORMFISH,
	TEX_COBBLE,
	TEX_FONT,
	TEX_BUTTON,
	TEX_BUTTON_PRESSED
};

enum {
	TILE_BRICK_CUBE = 1,
	TILE_BRICK_SLAB,
	TILE_BRICK_CORNER,
	TILE_BRICK_FLOOR,
	TILE_BRICK_PYRAMID,
	TILE_BRICK_SLOPE,
	TILE_BRICK_MONKEY,
	TILE_DIRT_FLOOR,
};

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

static void rotating_system(ecs_world_t *ecs) {
	ecs_query_t query = ecs_query1(ecs, "transform_c", "rotating_c", NULL);
	for (size_t i = 0; i < query.length; i++) {
		transform_c *transform = ECS_GET(ecs, query.entities[i], transform_c);
		rotating_c *rotating = ECS_GET(ecs, query.entities[i], rotating_c);

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
		printf("Failed to initialize SDL2: %s\n", SDL_GetError());
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
		printf("Failed to create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Make an OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
		printf("Failed to create OpenGL context: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to load OpenGL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	printf("OpenGL loaded\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	
	glViewport(0, 0, window_width, window_height);

	res_pack_t res_pack = {0};
	res_pack.render_width = 640;
	res_pack.render_height = 360;
	// res_pack.render_height = 480;
	res_pack.sky_color = (color_t){58, 49, 41, 255};
	// res_pack.sky_color = (color_t){2, 9, 23, 255};
	// res_pack.sky_color = (color_t){0, 0, 0, 255};
	res_pack.fog_color = (color_t){58, 49, 41, 255};
	res_pack.editor_color = (color_t){50, 50, 50, 255};

	// render_init(&res_pack);

	res_add_texture(&res_pack, TEX_BRICKS, load_tga("res/images/bricks.tga", true));
	res_add_texture(&res_pack, TEX_IRON_BARS, load_tga("res/images/iron_bars.tga", true));
	res_add_texture(&res_pack, TEX_TREE, load_tga("res/images/tree.tga", true));
	res_add_texture(&res_pack, TEX_BIRCH, load_tga("res/images/birch.tga", true));
	res_add_texture(&res_pack, TEX_DIRT, load_tga("res/images/dirt.tga", true));
	res_add_texture(&res_pack, TEX_CROSSHAIR, load_tga("res/images/crosshair.tga", true));
	res_add_texture(&res_pack, TEX_WORMFISH, load_tga("res/images/wormfish.tga", true));
	res_add_texture(&res_pack, TEX_COBBLE, load_tga("res/images/cobble.tga", false));
	res_add_texture(&res_pack, TEX_FONT, load_tga("res/images/font.tga", false));

	res_add_texture(&res_pack, TEX_BUTTON, load_tga("res/images/button.tga", false));
	res_add_texture(&res_pack, TEX_BUTTON_PRESSED, load_tga("res/images/button_pressed.tga", false));
	res_pack.button_tex_index = TEX_BUTTON;
	res_pack.button_pressed_tex_index = TEX_BUTTON_PRESSED;

	res_add_mesh_raw(&res_pack, MESH_QUAD, quad_vertices, sizeof(quad_vertices) / sizeof(vertex_t), quad_indices, sizeof(quad_indices) / sizeof(u32));
	res_add_mesh(&res_pack, MESH_CUBE, load_mesh("res/meshes/cube.mesh"));
	res_add_mesh(&res_pack, MESH_FLOOR, load_mesh("res/meshes/floor.mesh"));
	res_add_mesh(&res_pack, MESH_SLAB, load_mesh("res/meshes/slab.mesh"));
	res_add_mesh(&res_pack, MESH_SLOPE, load_mesh("res/meshes/slope.mesh"));
	res_add_mesh(&res_pack, MESH_PYRAMID, load_mesh("res/meshes/pyramid.mesh"));
	res_add_mesh(&res_pack, MESH_CORNER, load_mesh("res/meshes/corner.mesh"));
	res_add_mesh(&res_pack, MESH_MONKEY, load_mesh("res/meshes/monkey.mesh"));
	res_add_mesh(&res_pack, MESH_MUSHROOM, load_mesh("res/meshes/mushroom.mesh"));

	res_pack.tiles[TILE_BRICK_CUBE] = (tile_t) {
		.mesh_index = MESH_CUBE,
		// .texture_index = TEX_BRICKS,
		.texture_index = TEX_COBBLE,
		.rotation = {0},
	};
	
	res_pack.tiles[TILE_BRICK_FLOOR] = (tile_t) {
		.mesh_index = MESH_FLOOR,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_BRICK_SLAB] = (tile_t) {
		.mesh_index = MESH_SLAB,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_BRICK_SLOPE] = (tile_t) {
		.mesh_index = MESH_SLOPE,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_BRICK_PYRAMID] = (tile_t) {
		.mesh_index = MESH_PYRAMID,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_BRICK_CORNER] = (tile_t) {
		.mesh_index = MESH_CORNER,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_BRICK_MONKEY] = (tile_t) {
		.mesh_index = MESH_MONKEY,
		.texture_index = TEX_BRICKS,
		.rotation = {0},
	};

	res_pack.tiles[TILE_DIRT_FLOOR] = (tile_t) {
		.mesh_index = MESH_FLOOR,
		// .mesh_index = MESH_MUSHROOM,
		.texture_index = TEX_DIRT,
		.rotation = {0},
	};

	// font_t font = {0};
	font_init(&res_pack.font, &res_pack, TEX_FONT);

	grid_t grid = {0};
	grid.width = 16;
	grid.height = 16;
	grid.depth = 16;

	for (int z = 0; z < 16; z++) {
		for (int x = 0; x < 16; x++) {
			grid_set_cell(&grid, TILE_DIRT_FLOOR, x, 0, z);
		}
	}

	grid_set_cell(&grid, TILE_BRICK_CUBE, 0, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 1, 0, 0);
	// grid_set_cell(&grid, TILE_BRICK_CUBE, 2, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 3, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 4, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 5, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 6, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 7, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 8, 0, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 0);

	grid_set_cell(&grid, TILE_BRICK_CUBE, 7, 1, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 8, 1, 0);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 1, 0);

	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 1);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 2);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 3);
	grid_set_cell(&grid, TILE_BRICK_CUBE, 9, 0, 4);
	
	grid_set_cell(&grid, TILE_BRICK_SLOPE, 10, 0, 4);
	grid_set_cell(&grid, TILE_BRICK_FLOOR, 11, 0, 4);
	grid_set_cell(&grid, TILE_BRICK_PYRAMID, 12, 0, 4);
	grid_set_cell(&grid, TILE_BRICK_SLAB, 13, 0, 4);
	grid_set_cell(&grid, TILE_BRICK_CORNER, 14, 0, 4);
	grid_set_cell(&grid, TILE_BRICK_MONKEY, 15, 0, 4);

	camera = create_camera();

	bool edit_mode = false;
	bool fullscreen = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Don't lock fps
	// SDL_GL_SetSwapInterval(0);

	ecs_world_t ecs = ecs_create();
	ECS_REGISTER_COMPONENT(&ecs, transform_c);
	ECS_REGISTER_COMPONENT(&ecs, sprite_c);
	ECS_REGISTER_COMPONENT(&ecs, mesh_c);
	ECS_REGISTER_COMPONENT(&ecs, rotating_c);

	for (int i = 0; i < 100; i++) {
		entity_t tree = ecs_new(&ecs);
		transform_c *transform = ECS_SET(&ecs, tree, transform_c, {0});
		transform->position[0] = 10.0f;
		transform->position[1] = 0.0f;
		transform->position[2] = 5.0f + i;

		ECS_SET(&ecs, tree, sprite_c, {TEX_BIRCH, true, 1.0f, 2.0f});
	}

	{
		entity_t tree = ecs_new(&ecs);
		transform_c *transform = ECS_SET(&ecs, tree, transform_c, {0});
		transform->position[0] = 5.0f;
		transform->position[1] = 0.0f;
		transform->position[2] = 5.0f;

		ECS_SET(&ecs, tree, sprite_c, {TEX_WORMFISH, true, 2.0f, 1.0f});
	}

	// Iron bars
	for (int i = 0; i < 1; i++) {
		entity_t bars = ecs_new(&ecs);
		transform_c *transform = ECS_SET(&ecs, bars, transform_c, {0});
		transform->position[0] = 2.0f;
		transform->position[1] = 0.0f;
		transform->position[2] = 0.0f + i;

		// transform->rotation[1] = 90.0f;
		ECS_SET(&ecs, bars, sprite_c, {TEX_IRON_BARS, false, 1.0f, 1.0f});

	}

	{
		entity_t monke = ecs_new(&ecs);
		transform_c *transform = ECS_SET(&ecs, monke, transform_c, {0});
		mesh_c *mesh = ECS_SET(&ecs, monke, mesh_c, {0});
		rotating_c *rotating = ECS_SET(&ecs, monke, rotating_c, {0});

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
			render_image(&res_pack, PIVOT_CENTER, 6, res_pack.render_width / 2, res_pack.render_height / 2);

			// render_image(res_pack, PIVOT_TOP_LEFT, 6, 1, 1);
			render_image(&res_pack, PIVOT_TOP_LEFT, 4, 0, 0);
			render_image(&res_pack, PIVOT_TOP_LEFT, 3, 0, -4);
			render_image(&res_pack, PIVOT_TOP_LEFT, 4, 640-64, 0);
			render_image(&res_pack, PIVOT_TOP_LEFT, 3, 640-64, -4);

			render_mesh_isometric(&res_pack, res_pack.meshes[MESH_CUBE], 1, 100, 100);
			render_mesh_isometric(&res_pack, res_pack.meshes[MESH_SLAB], 2, 200, 100);

			render_image_ex(&res_pack, TEX_COBBLE, PIVOT_TOP_LEFT, (rect_t){16, 16, 16, 16}, 0, 0, 50.0f, (vec2){1.0f, 1.0f});

			// gui_print(&res_pack, &font, "JATOCCHHH DIT IS EEN STUK TEXT YES HET KANKERWERKTS", 0, 0);
			// gui_print(&res_pack, &font, "hier nog een paar kleine letters", 0, 0);
			gui_print(&res_pack, &res_pack.font, "DREAM SIMULATOR v0.1", 1, 0);
			// gui_print(&res_pack, &font, "!@#$%^&*()-=", 0, 0);

			render_end_frame_buffer(&res_pack);
		}

		SDL_GL_SwapWindow(window);

		// SDL_Delay(1000/FPS);
	}

	printf("Quitting\n");

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}