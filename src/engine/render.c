#include "render.h"

static u32 shader_program;

static camera_t camera = {0};

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

// static vertex_t slope_vertices[] = {
// 	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
// 	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
// 	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},

// 	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
// 	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
// 	{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
// 	{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
// };

// static u32 slope_indices[] = {
// 	// Bottom face
// 	0, 1, 5,
// 	0, 5, 4,

// 	// Back face
// 	4, 5, 6,
// 	4, 6, 7,

// 	// Slope face
// 	0, 6, 7,
// 	0, 6, 1,

// 	// Side faces
// };

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

static level_t level = {0};

static res_pack_t res_pack = {0};

static camera_t create_camera() {
	camera_t camera = {0};

	glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);

	camera.pitch = 0.0f;
	camera.yaw = -90.0f;

	return camera;
}

void render_init() {
	level.width = 16;
	level.height = 16;
	level.depth = 16;

	res_add_texture(&res_pack, 1, "res/images/green.tga");
	res_add_texture(&res_pack, 2, "res/images/test.tga");

	res_add_mesh_raw(&res_pack, 1, cube_vertices, sizeof(cube_vertices) / sizeof(vertex_t), cube_indices, sizeof(cube_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, 2, slope_vertices, sizeof(slope_vertices) / sizeof(vertex_t), slope_indices, sizeof(slope_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, 3, floor_vertices, sizeof(floor_vertices) / sizeof(vertex_t), floor_indices, sizeof(floor_indices) / sizeof(u32));

	res_pack.tiles[1] = (tile_t){
		.mesh_index = 1,
		.texture_index = 1
	};

	res_pack.tiles[2] = (tile_t){
		.mesh_index = 1,
		.texture_index = 2
	};

	res_pack.tiles[3] = (tile_t){
		.mesh_index = 2,
		.texture_index = 2
	};

	res_pack.tiles[4] = (tile_t){
		.mesh_index = 3,
		.texture_index = 1
	};

	level.map[6] = 1;
	level.map[2] = 1;
	level.map[4] = 1;
	level.map[0] = 2;
	level.map[8] = 3;
	// level.map[9] = 4;

	camera = create_camera();

    // shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment_tex.glsl");
}

void render() {
	float camera_speed = 0.4f;
	if (input_key_held(SDL_SCANCODE_W)) {
		vec3 temp;
		glm_vec3_scale(camera.front, camera_speed, temp);
		glm_vec3_add(camera.position, temp, camera.position);
	}
	if (input_key_held(SDL_SCANCODE_S)) {
		vec3 temp;
		glm_vec3_scale(camera.front, -camera_speed, temp);
		glm_vec3_add(camera.position, temp, camera.position);
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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    glUseProgram(shader_program);

	// View matrix
	mat4 view;
	vec3 temp;
	glm_vec3_add(camera.position, camera.front, temp);
	glm_lookat(camera.position, temp, camera.up, view);
	shader_set_mat4(shader_program, "view", &view);

	// Projection matrix
	mat4 projection;
	// glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	glm_perspective(glm_rad(60.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	// glm_perspective()
	// glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f, projection);
	shader_set_mat4(shader_program, "projection", &projection);

	shader_set_int(shader_program, "texture1", 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (i32 z = 0; z < level.depth; z++) {
		for (i32 y = 0; y < level.height; y++) {
			for (i32 x = 0; x < level.width; x++) {
				size_t tile_index = level_get_tile_index(&level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }
                
                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				shader_set_mat4(shader_program, "model", &model);

				tile_t tile = res_pack.tiles[tile_index];
				mesh_t mesh = res_pack.meshes[tile.mesh_index];
				
				glBindVertexArray(mesh.vao);

				glBindTexture(GL_TEXTURE_2D, tile.texture_index);
				glActiveTexture(GL_TEXTURE0);

				glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
				
				glBindVertexArray(0);
			}
		}
	}
}

void render_input(SDL_Event event) {
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

		vec3 front;
		front[0] = cosf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));
		front[1] = sinf(glm_rad(camera.pitch));
		front[2] = sinf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));

		glm_vec3_normalize(front);
		glm_vec3_copy(front, camera.front);
	}
}