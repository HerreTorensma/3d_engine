#include "render.h"

static u32 game_shader = 0;
static u32 basic_shader = 0;
static u32 ortho_shader = 0;
static u32 gui_shader = 0;

static u32 fbo = 0;
static u32 fbo_tex = 0;

static mat4 ortho_views[6] = {0};

static mat4 isometric_view = {0};

static vec3 *global_position = NULL;
static ecs_world_t *global_ecs = NULL;

static vertex_t quad_vertices[] = {
    {{1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}}
};
static u32 quad_indices[] = {
    0, 1, 3,
    1, 2, 3
};

mesh_t quad_mesh = {0};

static void clear(color_t color) {
	glViewport(0, 0, window_width, window_height);

	vec4 gl_color = {0};
	color_to_gl_color(color, gl_color);
	glClearColor(gl_color[0], gl_color[1], gl_color[2], gl_color[3]);

	glViewport(x_offset, y_offset, viewport_width, viewport_height);
}

inline static void render_mesh(res_pack_t *res_pack, mesh_t *mesh, index_t texture_index) {
	glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, res_pack->texture_ids[texture_index]);
	
	glBindTexture(GL_TEXTURE_2D, texture_index);
	// glBindTexture(GL_TEXTURE_2D, res_pack->textures[texture_index].id);

	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
}

static void init_frame_buffer(res_pack_t *res_pack) {
	// FBO and texture
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	u32 rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, res_pack->render_width, res_pack->render_height);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, res_pack->render_width, res_pack->render_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, res_pack->render_width, res_pack->render_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Frame buffer not complete\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void render_init(res_pack_t *res_pack) {
	{
		// vec3 camera_pos = {1.0f, 1.0f, 1.0f};  // Camera position (above looking down)
		// vec3 camera_pos = {0.0f, 10.0f, 10.0f};  // Camera position (above looking down)
		vec3 camera_pos = {0.0f, 20.0f, 20.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 1.0f, 0.0f};  // Up vector (in Blender's coordinate system)

		camera_pos[0] *= cosf(glm_rad(45.0f));
		camera_pos[1] *= sinf(glm_rad(30.0f));
		camera_pos[2] *= sinf(glm_rad(45.0f));

		glm_lookat(camera_pos, target_pos, up_vector, isometric_view);
	}

	{
		vec3 camera_pos = {0.0f, 10.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 0.0f, -1.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, ortho_views[ORTHO_TOP]);
	}

	{
		vec3 camera_pos = {10.0f, 0.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 1.0f, 0.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, ortho_views[ORTHO_FRONT]);
	}

	{
		vec3 camera_pos = {-10.0f, 0.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 1.0f, 0.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, ortho_views[ORTHO_BACK]);
	}

    game_shader = create_shader_program("res/shaders/game/vert.glsl", "res/shaders/game/frag.glsl");
    basic_shader = create_shader_program("res/shaders/2d/vert.glsl", "res/shaders/2d/frag.glsl");
    ortho_shader = create_shader_program("res/shaders/editor/vert.glsl", "res/shaders/editor/frag.glsl");
    gui_shader = create_shader_program("res/shaders/gui/vert.glsl", "res/shaders/gui/frag.glsl");
	
	init_frame_buffer(res_pack);

	// Load the quad
	quad_mesh = load_mesh_raw(quad_vertices, sizeof(quad_vertices) / sizeof(vertex_t), quad_indices, sizeof(quad_indices) / sizeof(u32));
	send_mesh_to_gpu(&quad_mesh);
}

void render_start_frame_buffer(res_pack_t *res_pack) {
	// Frame buffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, res_pack->render_width, res_pack->render_height);
}

void render_end_frame_buffer(res_pack_t *res_pack) {
	// glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// More framebuffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Bind default framebuffer
	glViewport(0, 0, window_width, window_height);  // Set viewport back to full resolution
	glDisable(GL_DEPTH_TEST);

	glUseProgram(basic_shader);

	render_mesh(res_pack, &quad_mesh, fbo_tex);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

static i32 compare_by_distance(const void *a, const void *b) {
	entity_t entity1 = *(entity_t *)a;
	entity_t entity2 = *(entity_t *)b;

	transform_c *transform1 = ecs_get(global_ecs, entity1, TRANSFORM_C);
	transform_c *transform2 = ecs_get(global_ecs, entity2, TRANSFORM_C);

	float distance1 = ((*global_position[0] - transform1->position[0]) * (*global_position[0] - transform1->position[0])) + ((*global_position[1] - transform1->position[1]) * (*global_position[1] - transform1->position[1])) + ((*global_position[2] - transform1->position[2]) * (*global_position[2] - transform1->position[2]));
	float distance2 = ((*global_position[0] - transform2->position[0]) * (*global_position[0] - transform2->position[0])) + ((*global_position[1] - transform2->position[1]) * (*global_position[1] - transform2->position[1])) + ((*global_position[2] - transform2->position[2]) * (*global_position[2] - transform2->position[2]));

	if (distance1 > distance2) {
		return -1;
	} else if (distance1 < distance2) {
		return 1;
	} else {
		return 0;
	}
}

static void render_mesh_transform(res_pack_t *res_pack, transform_t *transform, index_t mesh_index, index_t tex_index) {
	mesh_t *mesh = &res_pack->meshes[mesh_index];

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, transform->position);
	glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

	glm_rotate(model, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
	glm_rotate(model, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
	glm_rotate(model, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});

	shader_set_mat4(game_shader, "model", &model);

	render_mesh(res_pack, mesh, tex_index);
}

static void render_mesh_components(res_pack_t *res_pack, ecs_world_t *ecs) {
	ecs_query_t query = ecs_query(ecs, TRANSFORM_C, MESH_C, -1);
	for (size_t i = 0; i < query.len; i++) {
		mesh_c *mesh_component = ecs_get(ecs, query.entities[i], MESH_C);
		transform_c *transform = ecs_get(ecs, query.entities[i], TRANSFORM_C);

		mesh_t mesh = res_pack->meshes[mesh_component->mesh_index];

		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, transform->position);
		glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});

		glm_rotate(model, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});

		shader_set_mat4(game_shader, "model", &model);

		render_mesh(res_pack, &mesh, mesh_component->texture_index);
	}
}

static void render_sprite_transform(res_pack_t *res_pack, transform_t *transform, camera_t *camera, sprite_c *sprite) {
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3){transform->position[0], transform->position[1] + ((sprite->y_scale - 1) / 2), transform->position[2]});
	glm_scale(model, (vec3){0.5f * sprite->x_scale, 0.5f * sprite->y_scale, 0.5f * sprite->x_scale});

	glm_rotate(model, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
	glm_rotate(model, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
	glm_rotate(model, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});

	if (sprite->billboard) {
		vec3 camera_forward = {camera->front[0], 0.0f, camera->front[2]};
		glm_normalize(camera_forward);

		float angle = atan2(camera_forward[0], camera_forward[2]);

		glm_rotate(model, angle, (vec3){0.0f, 1.0f, 0.0f});
	}

	shader_set_mat4(game_shader, "model", &model);

	render_mesh(res_pack, &quad_mesh, sprite->texture_index);
}

static void render_sprite_components(res_pack_t *res_pack, ecs_world_t *ecs, camera_t *camera) {
	// Transparent stuff
	ecs_query_t query = ecs_query(ecs, TRANSFORM_C, SPRITE_C, -1);
	qsort(query.entities, query.len, sizeof(entity_t), compare_by_distance);
	for (size_t i = 0; i < query.len; i++) {
		sprite_c *sprite = ecs_get(ecs, query.entities[i], SPRITE_C);
		transform_c *transform = ecs_get(ecs, query.entities[i], TRANSFORM_C);

		mesh_t mesh = quad_mesh;

		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, (vec3){transform->position[0], transform->position[1] + ((sprite->y_scale - 1) / 2), transform->position[2]});
		// glm_scale(model, (vec3){0.5f * sprite->x_scale, 0.5f * sprite->y_scale, 0.5f});
		glm_scale(model, (vec3){0.5f * sprite->x_scale, 0.5f * sprite->y_scale, 0.5f * sprite->x_scale});

		glm_rotate(model, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});

		if (sprite->billboard) {
			// vec3 direction = {camera->position[0] - transform->position[0], 0.0f, camera->position[2] - transform->position[2]};
			// glm_normalize(direction);
			// float angle = atan2(direction[0], direction[2]);
			// glm_rotate(model, angle, (vec3){0.0f, 1.0f, 0.0f});

			    // Get the forward direction of the camera projected onto the XZ plane
			vec3 camera_forward = {camera->front[0], 0.0f, camera->front[2]};
			glm_normalize(camera_forward);

			// Calculate the angle between the camera's forward direction and the world forward (Z-axis)
			float angle = atan2(camera_forward[0], camera_forward[2]);

			// Rotate the sprite to align with the camera's forward direction
			glm_rotate(model, angle, (vec3){0.0f, 1.0f, 0.0f});
		}

		shader_set_mat4(game_shader, "model", &model);

		render_mesh(res_pack, &mesh, sprite->texture_index);
	}
}

static void render_grid(res_pack_t *res_pack, grid_t *grid) {
	for (u32 z = 0; z < grid->depth; z++) {
		for (u32 y = 0; y < grid->height; y++) {
			for (u32 x = 0; x < grid->width; x++) {
				tile_t tile = grid_get_cell(grid, x, y, z);
				if (!tile.occupied) {
					continue;
				}

				mesh_t mesh = res_pack->meshes[tile.mesh_index];
                
                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
				shader_set_mat4(game_shader, "model", &model);
				
				// vec3 fog_color = {1.0f, 0.0f, 0.0f};
				// vec3 fog_color = {0.231f, 0.2f, 0.149f};
				// vec3 fog_color = {0.8f, 0.8f, 0.8f};
				// vec3 fog_color = {0.0f, 0.0f, 0.0f};

				// vec3 fog_color = {0.227f, 0.192f, 0.161f};

				vec4 fog_color = {0};
				color_to_gl_color(res_pack->fog_color, fog_color);
				
				shader_set_vec3(game_shader, "fogColor", &fog_color);

				render_mesh(res_pack, &mesh, tile.texture_index);
			}
		}
	}
}

void render_image(res_pack_t *res_pack, index_t texture_index, i32 x, i32 y, color_t color) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(gui_shader);

	mat4 projection = {0};
	// glm_ortho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f, projection);
	glm_ortho(0.0f, res_pack->render_width, res_pack->render_height, 0.0f, -1.0f, 1.0f, projection);

	texture_t texture = res_pack->textures[texture_index];

	mat4 model = {0};
	glm_mat4_identity(model);

	// glm_translate(model, (vec3){x_offset, y_offset, 0.0f});
	// glm_translate(model, (vec3){(float)(x * screen_scale), (float)(y * screen_scale), 0.0f});
	glm_translate(model, (vec3){(float)x, (float)y, 0.0f});
	
	glm_scale(model, (vec3){(float)texture.width / 2.0f, (float)texture.height / 2.0f, 0.0f});
	// glm_scale(model, (vec3){(float)screen_scale, (float)screen_scale, 0.0f});

	glm_translate(model, (vec3){1.0f, 1.0f, 0.0f});

	shader_set_mat4(gui_shader, "model", &model);
	shader_set_mat4(gui_shader, "projection", &projection);

	shader_set_vec2(gui_shader, "tex_scale", (vec2){1.0f, 1.0f});
	shader_set_vec2(gui_shader, "tex_offset", (vec2){0.0f, 0.0f});

	vec4 gl_color = {0};
	color_to_gl_color(color, gl_color);
	shader_set_vec4(gui_shader, "color1", &gl_color);

	render_mesh(res_pack, &quad_mesh, texture_index);
}

void render_image_rect(res_pack_t *res_pack, index_t texture_index, rect_t src, rect_t dst, color_t color) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(gui_shader);

	mat4 projection = {0};
	// glm_ortho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f, projection);
	glm_ortho(0.0f, res_pack->render_width, res_pack->render_height, 0.0f, -1.0f, 1.0f, projection);

	texture_t texture = res_pack->textures[texture_index];

	mat4 model = {0};
	glm_mat4_identity(model);

	// glm_translate(model, (vec3){x_offset, y_offset, 0.0f});
	// glm_translate(model, (vec3){(float)dst.x * screen_scale, (float)dst.y * screen_scale, 0.0f});
	glm_translate(model, (vec3){(float)dst.x, (float)dst.y, 0.0f});
	
	// Scale according to the dst rect
	glm_scale(model, (vec3){(float)dst.w / 2.0f, (float)dst.h / 2.0f, 0.0f});
	// glm_scale(model, (vec3){(float)screen_scale, (float)screen_scale, 0.0f});

	// Draw from top left instead of center
	glm_translate(model, (vec3){1.0f, 1.0f, 0.0f});

	shader_set_mat4(gui_shader, "model", &model);
	shader_set_mat4(gui_shader, "projection", &projection);

	vec2 tex_scale = {
		(float)src.w / (float)texture.width,
		(float)src.h / (float)texture.height,
	};
	vec2 tex_offset = {
		(float)src.x / (float)texture.width,
		(float)src.y / (float)texture.height,
	};
	shader_set_vec2(gui_shader, "tex_scale", &tex_scale);
	shader_set_vec2(gui_shader, "tex_offset", &tex_offset);

	vec4 gl_color = {0};
	color_to_gl_color(color, gl_color);
	shader_set_vec4(gui_shader, "color1", &gl_color);

	render_mesh(res_pack, &quad_mesh, texture_index);
}

void render_mesh_isometric(res_pack_t *res_pack, mesh_t mesh, index_t texture_index, i32 x, i32 y, float scale) {
    glEnable(GL_DEPTH_TEST);

    glUseProgram(gui_shader);

    mat4 projection = {0};

    // glm_ortho(
    //     0.0f, (float)(window_width / screen_scale),
    //     0.0f, (float)(window_height / screen_scale),
    //     0.1f, 1024.0f,
    //     projection
    // );
	glm_ortho(0.0f, res_pack->render_width, 0.0f, res_pack->render_height, 0.1f, 1024.0f, projection);

    mat4 model = {0};
    glm_mat4_identity(model);

	glm_translate(model, (vec3){x, res_pack->render_height - y, -16.0f});
	// glm_translate(model, (vec3){x_offset / screen_scale, y_offset / screen_scale, 0.0f});
    // glm_translate(model, (vec3){
    //     (float)x,
    //     (float)((window_height / screen_scale) - y),
    //     -16.0f,
    // });
	// glm_translate(model, (vec3){
    //     (float)(x + x_offset / screen_scale),
    //     (float)((window_height / screen_scale) - (y + y_offset / screen_scale)),
    //     -16.0f,
    // });

	glm_scale(model, (vec3){scale, scale, scale});

    glm_rotate(model, glm_rad(35.264f), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(45.0f), (vec3){0.0f, 1.0f, 0.0f});

    shader_set_mat4(gui_shader, "model", &model);
    shader_set_mat4(gui_shader, "projection", &projection);

    render_mesh(res_pack, &mesh, texture_index);
}

void render_grid_ortho(res_pack_t *res_pack, grid_t *grid, enum ortho_view orientation, float zoom, mat4 *projection, i32 min_y, i32 max_y, bool enable_transparency) {
	clear(res_pack->editor_color);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    glUseProgram(ortho_shader);

    shader_set_mat4(ortho_shader, "view", &ortho_views[orientation]);
    // shader_set_mat4(ortho_shader, "view", &isometric_view);
	
	shader_set_mat4(ortho_shader, "projection", projection);

	shader_set_int(ortho_shader, "texture1", 0);

	shader_set_int(ortho_shader, "enable_transparency", enable_transparency);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (u32 z = 0; z < grid->depth; z++) {
		for (u32 y = min_y; y <= max_y; y++) {
			for (u32 x = 0; x < grid->width; x++) {
				tile_t tile = grid_get_cell(grid, x, y, z);
				if (!tile.occupied) {
					continue;
				}

				mesh_t mesh = res_pack->meshes[tile.mesh_index];

                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
				shader_set_mat4(ortho_shader, "model", &model);
				
				render_mesh(res_pack, &mesh, tile.texture_index);
			}
		}
	}
}

void render_game(res_pack_t *res_pack, grid_t *grid, ecs_world_t *ecs, vec3 position, camera_t *camera) {
	global_position = &position;
	global_ecs = ecs;

	clear(res_pack->sky_color);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(game_shader);

	// View matrix
	mat4 view = {0};
	vec3 temp = {0};
	glm_vec3_add(position, camera->front, temp);
	glm_lookat(position, temp, camera->up, view);
	shader_set_mat4(game_shader, "view", &view);

	// Projection matrix
	mat4 projection = {0};
	glm_perspective(glm_rad(60.0f), (float)(viewport_width) / (float)(viewport_height), 0.1f, 100.0f, projection);
	
	shader_set_mat4(game_shader, "projection", &projection);

	shader_set_int(game_shader, "texture1", 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	render_grid(res_pack, grid);

	render_mesh_components(res_pack, ecs);

	render_sprite_components(res_pack, ecs, camera);
}