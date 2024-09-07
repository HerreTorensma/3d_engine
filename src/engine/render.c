#include "render.h"

static u32 shader_program;
static u32 vbo;
static u32 vao;

static camera_t camera = {0};

static float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f,
};

void render_init() {
	// camera.position = (vec3){0.0f, 0.0f, 3.0f};
	// camera.front = (vec3){0.0f, 0.0f, -1.0f};
	// camera.up = (vec3){0.0f, 1.0f, 0.0f};

	glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);

	camera.pitch = 0.0f;
	camera.yaw = -90.0f;

    shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
}

void render() {
	if (input_key_held(SDL_SCANCODE_W)) {
		vec3 temp;
		glm_vec3_scale(camera.front, 0.3f, temp);
		glm_vec3_add(camera.position, temp, camera.position);
	}
	if (input_key_held(SDL_SCANCODE_S)) {
		vec3 temp;
		glm_vec3_scale(camera.front, -0.3f, temp);
		glm_vec3_add(camera.position, temp, camera.position);
	}

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
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
	// glm_ortho();
	shader_set_mat4(shader_program, "projection", &projection);

	// Model matrix
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){0});
	shader_set_mat4(shader_program, "model", &model);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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