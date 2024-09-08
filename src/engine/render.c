#include "render.h"

static u32 shader_program;

static camera_t camera = {0};

static vertex_t cube_vertices[] = {
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},

	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
};

static u32 cube_indices[] = {
	// Bottom face
	0, 1, 5,
	0, 5, 4,

	// Top face
	2, 3, 7,
	2, 7, 6,

	// Front face
	0, 1, 2,
	0, 2, 3,

	// Back face
	4, 5, 6,
	4, 6, 7,

	// Left face
	0, 3, 7,
	0, 7, 4,

	// Right face
	1, 2, 6,
	1, 6, 5
};

static vertex_t slope_vertices[] = {
	{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},

	{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},
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

	// Side faces
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

	res_add_mesh_raw(&res_pack, 1, cube_vertices, sizeof(cube_vertices) / sizeof(vertex_t), cube_indices, sizeof(cube_indices) / sizeof(u32));
	res_add_mesh_raw(&res_pack, 2, slope_vertices, sizeof(slope_vertices) / sizeof(vertex_t), slope_indices, sizeof(slope_indices) / sizeof(u32));

	level.map[0] = 1;
	level.map[2] = 1;
	level.map[4] = 1;
	level.map[6] = 2;

	camera = create_camera();

    shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// View matrix
	mat4 view;
	vec3 temp;
	glm_vec3_add(camera.position, camera.front, temp);
	glm_lookat(camera.position, temp, camera.up, view);
	shader_set_mat4(shader_program, "view", &view);

	// Projection matrix
	mat4 projection;
	glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	shader_set_mat4(shader_program, "projection", &projection);

    glUseProgram(shader_program);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (i32 z = 0; z < level.depth; z++) {
		for (i32 y = 0; y < level.height; y++) {
			for (i32 x = 0; x < level.width; x++) {
				// Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				shader_set_mat4(shader_program, "model", &model);

				mesh_t mesh = res_pack.meshes[level.map[z * level.width * level.height + y * level.width + x]];
				
				glBindVertexArray(mesh.vao);

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