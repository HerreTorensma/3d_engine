#include "render.h"

static u32 shader_program;

static camera_t camera = {0};

static bool edit_mode = false;
static float edit_zoom = 10.0f;

static mat4 top_view;
static mat4 side_view;

static camera_t create_camera() {
	camera_t camera = {0};

	glm_vec3_copy((vec3){0.0f, 0.0f, 3.0f}, camera.position);
	glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.front);
	glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera.up);

	camera.pitch = 0.0f;
	camera.yaw = -90.0f;

	return camera;
}

void render_init(void) {
	camera = create_camera();

    // shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment_tex.glsl");

	{
		vec3 camera_pos = {0.0f, 10.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 0.0f, -1.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, top_view);
	}

	{
		vec3 camera_pos = {10.0f, 0.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 1.0f, 0.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, side_view);
	}
}

void render(res_pack_t *res_pack, level_t *level) {
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

	if (input_key_pressed(SDL_SCANCODE_E)) {
		edit_mode = !edit_mode;
	}

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_program);

	// View matrix
	mat4 view;

	// Projection matrix
	mat4 projection;

	if (edit_mode) {
		shader_set_mat4(shader_program, "view", &top_view);
		// shader_set_mat4(shader_program, "view", &side_view);

		float aspect = (float)window_width / (float)window_height;

		float left = -edit_zoom;
		float right = edit_zoom;

		float ortho_height = edit_zoom / aspect;
		float bottom = -ortho_height;
		float top = ortho_height;

		// glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f, projection);
		glm_ortho(left, right, bottom, top, 0.1f, 100.0f, projection);
	} else {
		vec3 temp;
		glm_vec3_add(camera.position, camera.front, temp);
		glm_lookat(camera.position, temp, camera.up, view);
		shader_set_mat4(shader_program, "view", &view);

		// glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
		glm_perspective(glm_rad(60.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	}
	
	shader_set_mat4(shader_program, "projection", &projection);

	shader_set_int(shader_program, "texture1", 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (i32 z = 0; z < level->depth; z++) {
		for (i32 y = 0; y < level->height; y++) {
			for (i32 x = 0; x < level->width; x++) {
				size_t tile_index = level_get_tile_index(level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }
                
                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				shader_set_mat4(shader_program, "model", &model);

				tile_t tile = res_pack->tiles[tile_index];
				mesh_t mesh = res_pack->meshes[tile.mesh_index];
				
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

		vec3 front = {0};
		front[0] = cosf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));
		// front[1] = sinf(glm_rad(camera.pitch));
		front[2] = sinf(glm_rad(camera.yaw)) * cosf(glm_rad(camera.pitch));

		glm_vec3_normalize(front);
		glm_vec3_copy(front, camera.front);
	}

	if (event.type == SDL_MOUSEWHEEL) {
		edit_zoom -= (float)event.wheel.y;
	}
}