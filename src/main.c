#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"

// static vertex_t quad_vertices[] = {
//     {{1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},  // top right
//     {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // bottom right
//     {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},  // bottom left
//     {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}}   // top left 
// };

static vertex_t big_quad_vertices[] = {
    {{1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}},  // top right
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // bottom left
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}}   // top left 
};
static u32 big_quad_indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

static vertex_t quad_vertices[] = {
    {{0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},  // top right
    {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},  // bottom right
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},  // bottom left
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}   // top left 
};
static u32 quad_indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

static vertex_t cube_vertices[] = {
    // Front face
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // Bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}, // Top-right
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}}, // Top-left

    // Back face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // Top-left
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}}, // Top-right
    {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}}, // Bottom-right

    // Left face
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}}, // Bottom-right
    {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f}}, // Top-left
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}}, // Top-right

    // Right face
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // Top-left
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f}}, // Top-right
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // Bottom-right

    // Top face
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}}, // Top-left
    {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}}, // Bottom-right
    {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}}, // Top-right

    // Bottom face
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // Top-right
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // Top-left
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // Bottom-right
};

static u32 cube_indices[] = {
	// Front face
	0, 1, 2, 2, 3, 0,
	// Back face
	4, 5, 6, 6, 7, 4,
	// Left face
	8, 9, 10, 10, 11, 8,
	// Right face
	12, 13, 14, 14, 15, 12,
	// Top face
	16, 17, 18, 18, 19, 16,
	// Bottom face
	20, 21, 22, 22, 23, 20,
};

static vertex_t floor_vertices[] = {
	// Bottom face
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}}, // Top-right
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // Top-left
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f}}, // Bottom-left
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f}}, // Bottom-right
};

static u32 floor_indices[] = {
	0, 1, 2,
	2, 3, 0,
};

// static vertex_t cross_vertices[] = {
//     // First quad (aligned along YZ plane, centered at the origin)
//     {{ 0.0f, -0.5f, -0.5f}, {0.0f, 0.0f}},  // Bottom-left
//     {{ 0.0f,  0.5f, -0.5f}, {1.0f, 0.0f}},  // Top-left
//     {{ 0.0f,  0.5f,  0.5f}, {1.0f, 1.0f}},  // Top-right
//     {{ 0.0f, -0.5f,  0.5f}, {0.0f, 1.0f}},  // Bottom-right

//     // Second quad (aligned along XY plane, centered at the origin)
//     {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
//     {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
//     {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},  // Top-right
//     {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},  // Top-left
// };

// static u32 cross_indices[] = {
// 	0, 1, 2,
// 	2, 3, 0,

// 	4, 5, 6,
// 	6, 7, 4,
// };

static vertex_t cross_vertices[] = {

    // {{ 0.0f, -0.5f, -0.5f}, {0.0f, 0.0f}},  // Bottom-left
    // {{ 0.0f,  0.5f, -0.5f}, {1.0f, 0.0f}},  // Top-left
    // {{ 0.0f,  0.5f,  0.5f}, {1.0f, 1.0f}},  // Top-right
    // {{ 0.0f, -0.5f,  0.5f}, {0.0f, 1.0f}},  // Bottom-right

    // {{ -0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
    // {{ -0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},  // Top-left
    // {{ 0.5f,  0.5f,  0.0f}, {1.0f, 1.0f}},  // Top-right
    // {{ 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f}},  // Bottom-right

    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f}},  // Top-right
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},  // Bottom-right
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}},  // Top-left

    // Second quad (YZ plane, scaled to 0.5)
    {{ 0.0f,  0.5f,  0.5f}, {1.0f, 1.0f}},  // Top-right
    {{ 0.0f, -0.5f,  0.5f}, {1.0f, 0.0f}},  // Bottom-right
    {{ 0.0f, -0.5f, -0.5f}, {0.0f, 0.0f}},  // Bottom-left
    {{ 0.0f,  0.5f, -0.5f}, {0.0f, 1.0f}},  // Top-left
};

static u32 cross_indices[] = {
    // First quad (YZ plane)
    0, 1, 2,
    2, 3, 0,

    // Second quad (XY plane)
    4, 5, 6,
    6, 7, 4,
};

static vertex_t slope_vertices[] = {
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // 0 (Bottom left)
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},  // 1 (Bottom right)
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},   // 2 (Top right)
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},  // 3 (Top left)

    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},   // 4 (Bottom left)
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},    // 5 (Bottom right)
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},     // 6 (Top right)
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},    // 7 (Top left)
};

static u32 slope_indices[] = {
    // Bottom face
    0, 1, 5,
    0, 5, 4,

    // Back face
    4, 5, 6,
    4, 6, 7,

    // Slope face
    0, 6, 7,
    0, 6, 1,

    // Left face
    0, 7, 4,

    // Right face
    1, 5, 6,
};

static camera_t create_camera() {
	camera_t camera = {0};

	glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera.position);
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

void game_update() {
	float camera_speed = 0.1f;
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
	res_add_texture(&res_pack, 1, "res/images/green.tga");
	res_add_texture(&res_pack, 2, "res/images/test.tga");
	res_add_texture(&res_pack, 3, "res/images/fire.tga");
	res_add_texture(&res_pack, 4, "res/images/bricks.tga");
	res_add_texture(&res_pack, 5, "res/images/iron_bars.tga");

	res_add_mesh_raw(&res_pack, MESH_BIG_QUAD, big_quad_vertices, sizeof(big_quad_vertices) / sizeof(vertex_t), big_quad_indices, sizeof(big_quad_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, MESH_CUBE, cube_vertices, sizeof(cube_vertices) / sizeof(vertex_t), cube_indices, sizeof(cube_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, MESH_SLOPE, slope_vertices, sizeof(slope_vertices) / sizeof(vertex_t), slope_indices, sizeof(slope_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, MESH_FLOOR, floor_vertices, sizeof(floor_vertices) / sizeof(vertex_t), floor_indices, sizeof(floor_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, MESH_CROSS, cross_vertices, sizeof(cross_vertices) / sizeof(vertex_t), cross_indices, sizeof(cross_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, MESH_QUAD, quad_vertices, sizeof(quad_vertices) / sizeof(vertex_t), quad_indices, sizeof(quad_indices) / sizeof(u32));

	res_pack.tiles[1] = (tile_t){
		.mesh_index = MESH_CUBE,
		.texture_index = 4,
		.rotation = {0},
		.transparent = false,
	};

	res_pack.tiles[2] = (tile_t){
		.mesh_index = MESH_CUBE,
		.texture_index = 1,
		.rotation = {0},
		.transparent = true,
	};

	res_pack.tiles[3] = (tile_t){
		.mesh_index = MESH_SLOPE,
		.texture_index = 2,
		.rotation = {0.0f, 90.0f, 0.0f},
		.transparent = false,
	};

	res_pack.tiles[4] = (tile_t){
		.mesh_index = MESH_FLOOR,
		.texture_index = 1,
		.rotation = {0},
		.transparent = false,
	};

	res_pack.tiles[5] = (tile_t) {
		.mesh_index = MESH_CROSS,
		.texture_index = 3,
		.rotation = {0},
		.transparent = true,
	};

	res_pack.tiles[6] = (tile_t) {
		.mesh_index = MESH_QUAD,
		.texture_index = 3,
		.rotation = {0},
		.transparent = false,
	};

	res_pack.tiles[7] = (tile_t) {
		.mesh_index = MESH_QUAD,
		.texture_index = 5,
		.rotation = {0},
		.transparent = true,
	};

	level_t level = {0};
	level.width = 16;
	level.height = 16;
	level.depth = 16;

	// level.map[6] = 1;
	// level.map[2] = 1;
	// level.map[4] = 1;
	// level.map[0] = 2;
	// level.map[8] = 3;
	// level.map[9] = 4;
	// level.map[10] = 5;

	level_set_tile_index(&level, 1, 0, 0, 0);
	level_set_tile_index(&level, 1, 1, 0, 0);
	level_set_tile_index(&level, 7, 2, 0, 0);
	level_set_tile_index(&level, 7, 2, 0, 1);
	level_set_tile_index(&level, 1, 3, 0, 0);
	level_set_tile_index(&level, 1, 4, 0, 0);
	level_set_tile_index(&level, 1, 4, 1, 0);
	level_set_tile_index(&level, 1, 4, 0, 1);
	level_set_tile_index(&level, 1, 4, 0, 2);
	level_set_tile_index(&level, 1, 4, 0, 3);
	level_set_tile_index(&level, 1, 4, 0, 4);

	level_set_tile_index(&level, 1, 1, 1, 0);
	level_set_tile_index(&level, 1, 2, 1, 0);
	level_set_tile_index(&level, 1, 3, 1, 0);

	level_set_tile_index(&level, 1, 4, 1, 4);
	level_set_tile_index(&level, 1, 4, 2, 4);
	level_set_tile_index(&level, 1, 4, 3, 4);
	level_set_tile_index(&level, 1, 4, 4, 4);
	level_set_tile_index(&level, 1, 4, 5, 4);
	level_set_tile_index(&level, 1, 4, 6, 4);
	level_set_tile_index(&level, 1, 4, 7, 4);
	level_set_tile_index(&level, 1, 4, 8, 4);
	level_set_tile_index(&level, 1, 4, 9, 4);
	level_set_tile_index(&level, 1, 4, 10, 4);

	level_set_tile_index(&level, 2, 5, 0, 4);
	level_set_tile_index(&level, 3, 6, 0, 4);
	level_set_tile_index(&level, 4, 7, 0, 4);
	level_set_tile_index(&level, 5, 8, 0, 4);
	// level_set_tile_index(&level, 6, 9, 0, 4);

	camera = create_camera();

	bool edit_mode = false;
	bool fullscreen = false;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	// Don't lock fps
	// SDL_GL_SetSwapInterval(0);

	render_init();
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
			editor_update();
		} else {
			game_update();
		}

		if (edit_mode) {
			editor_render(&res_pack, &level);
		} else {
			render_level(&res_pack, &level, &camera);
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