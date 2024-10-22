#include "render.h"

static u32 game_shader = 0;
static u32 basic_shader = 0;
static u32 ortho_shader = 0;
static u32 gui_shader = 0;

static u32 fbo = 0;
static u32 fbo_tex = 0;

static mat4 ortho_views[6] = {0};

static mat4 isometric_view = {0};

static camera_t *global_camera = NULL;
static ecs_world_t *global_ecs = NULL;

static void clear(color_t color) {
	vec4 gl_color = {0};
	color_to_gl_color(color, gl_color);
	glClearColor(gl_color[0], gl_color[1], gl_color[2], gl_color[3]);
}

inline static void render_mesh(res_pack_t *res_pack, mesh_t *mesh, size_t texture_index) {
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
}


static void start_frame_buffer(res_pack_t *res_pack) {
	// Frame buffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, res_pack->render_width, res_pack->render_height);
}

static void end_frame_buffer(res_pack_t *res_pack) {
	// More framebuffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Bind default framebuffer
	glViewport(0, 0, window_width, window_height);  // Set viewport back to full resolution
	glDisable(GL_DEPTH_TEST);

	glUseProgram(basic_shader);

	render_mesh(res_pack, &(res_pack->meshes[MESH_QUAD]), fbo_tex);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_init(res_pack_t *res_pack) {
	{
		vec3 camera_pos = {10.0f, 10.0f, 10.0f};  // Camera position (above looking down)
		// vec3 camera_pos = {0.0f, 10.0f, 10.0f};  // Camera position (above looking down)
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
}

int compare_by_distance(const void *a, const void *b) {
	entity_t entity1 = *(entity_t *)a;
	entity_t entity2 = *(entity_t *)b;

	transform_c *transform1 = ECS_GET(global_ecs, entity1, transform_c);
	transform_c *transform2 = ECS_GET(global_ecs, entity2, transform_c);

	float distance1 = ((global_camera->position[0] - transform1->position[0]) * (global_camera->position[0] - transform1->position[0])) + ((global_camera->position[1] - transform1->position[1]) * (global_camera->position[1] - transform1->position[1])) + ((global_camera->position[2] - transform1->position[2]) * (global_camera->position[2] - transform1->position[2]));
	float distance2 = ((global_camera->position[0] - transform2->position[0]) * (global_camera->position[0] - transform2->position[0])) + ((global_camera->position[1] - transform2->position[1]) * (global_camera->position[1] - transform2->position[1])) + ((global_camera->position[2] - transform2->position[2]) * (global_camera->position[2] - transform2->position[2]));

	if (distance1 > distance2) {
		return -1;
	} else if (distance1 < distance2) {
		return 1;
	} else {
		return 0;
	}
}

static void render_mesh_components(res_pack_t *res_pack, ecs_world_t *ecs) {
	ecs_query_t query = ecs_query1(ecs, "transform_c", "mesh_c", NULL);
	for (size_t i = 0; i < query.length; i++) {
		mesh_c *mesh_component = ECS_GET(ecs, query.entities[i], mesh_c);
		transform_c *transform = ECS_GET(ecs, query.entities[i], transform_c);

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

static void render_sprite_components(res_pack_t *res_pack, ecs_world_t *ecs, camera_t *camera) {
	// Transparent stuff
	ecs_query_t query = ecs_query1(ecs, "transform_c", "sprite_c", NULL);
	qsort(query.entities, query.length, sizeof(entity_t), compare_by_distance);
	for (size_t i = 0; i < query.length; i++) {
		sprite_c *sprite = ECS_GET(ecs, query.entities[i], sprite_c);
		transform_c *transform = ECS_GET(ecs, query.entities[i], transform_c);

		mesh_t mesh = res_pack->meshes[MESH_QUAD];

		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, (vec3){transform->position[0], transform->position[1] + ((sprite->y_scale - 1) / 2), transform->position[2]});
		glm_scale(model, (vec3){0.5f * sprite->x_scale, 0.5f * sprite->y_scale, 0.5f});

		glm_rotate(model, glm_rad(transform->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
		glm_rotate(model, glm_rad(transform->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});

		if (sprite->billboard) {
			vec3 direction = {camera->position[0] - transform->position[0], 0.0f, camera->position[2] - transform->position[2]};
			glm_normalize(direction);
			float angle = atan2(direction[0], direction[2]);
			glm_rotate(model, angle, (vec3){0.0f, 1.0f, 0.0f});
		}

		shader_set_mat4(game_shader, "model", &model);

		render_mesh(res_pack, &mesh, sprite->texture_index);
	}
}

static void render_level() {

}

enum pivot {
	PIVOT_CENTER,
	PIVOT_TOP_LEFT,
};

static void render_image(res_pack_t *res_pack, enum pivot pivot, size_t texture_index, i32 x, i32 y) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(gui_shader);

	mat4 projection = {0};
	glm_ortho(0.0f, res_pack->render_width, res_pack->render_height, 0.0f, -1.0f, 1.0f, projection);

	texture_t texture = res_pack->textures[texture_index];

	mat4 model = {0};
	glm_mat4_identity(model);

	glm_translate(model, (vec3){(float)x, (float)y, 0.0f});
	
	glm_scale(model, (vec3){(float)texture.width / 2.0f, (float)texture.height / 2.0f, 0.0f});

	if (pivot == PIVOT_TOP_LEFT) {
		glm_translate(model, (vec3){1.0f, 1.0f, 0.0f});
	}

	shader_set_mat4(gui_shader, "model", &model);
	shader_set_mat4(gui_shader, "projection", &projection);

	render_mesh(res_pack, &res_pack->meshes[MESH_QUAD], texture_index);
}

void render_game(res_pack_t *res_pack, level_t *level, ecs_world_t *ecs, camera_t *camera) {
	// Frame buffer stuff
	start_frame_buffer(res_pack);

	global_camera = camera;
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
	glm_vec3_add(camera->position, camera->front, temp);
	glm_lookat(camera->position, temp, camera->up, view);
	shader_set_mat4(game_shader, "view", &view);

	// Projection matrix
	mat4 projection = {0};
	// glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	glm_perspective(glm_rad(60.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	
	shader_set_mat4(game_shader, "projection", &projection);

	shader_set_int(game_shader, "texture1", 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (i32 z = 0; z < level->depth; z++) {
		for (i32 y = 0; y < level->height; y++) {
			for (i32 x = 0; x < level->width; x++) {
				size_t tile_index = level_get_tile_index(level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }

				// References an entity and not a tile
				// It wil get collected later anyway so we can skip it
				if (tile_index > 255) {
					continue;
				}

				tile_t tile = res_pack->tiles[tile_index];

				mesh_t mesh = res_pack->meshes[tile.mesh_index];
                
                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
				// glm_translate(model, (vec3){x * 2.0, y * 2.0, z * 2.0});
				glm_rotate(model, glm_rad(tile.rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
				shader_set_mat4(game_shader, "model", &model);
				
				// vec3 fog_color = {1.0f, 0.0f, 0.0f};
				// vec3 fog_color = {0.231f, 0.2f, 0.149f};
				// vec3 fog_color = {0.8f, 0.8f, 0.8f};
				vec3 fog_color = {0.227f, 0.192f, 0.161f};
				// vec3 fog_color = {0.0f, 0.0f, 0.0f};
				
				shader_set_vec3(game_shader, "fogColor", &fog_color);

				render_mesh(res_pack, &mesh, tile.texture_index);
			}
		}
	}

	render_mesh_components(res_pack, ecs);

	render_sprite_components(res_pack, ecs, camera);


	render_image(res_pack, PIVOT_CENTER, 6, res_pack->render_width / 2, res_pack->render_height / 2);

	render_image(res_pack, PIVOT_TOP_LEFT, 6, 1, 1);


	end_frame_buffer(res_pack);
}

void render_level_ortho(res_pack_t *res_pack, level_t *level, enum ortho_view orientation, float zoom) {
	// start_frame_buffer();

	// glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	clear(res_pack->editor_color);
	// glClearColor(res_pack->editor_color.gl_color[0], res_pack->editor_color.gl_color[1], res_pack->editor_color.gl_color[2], res_pack->editor_color.gl_color[3]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    glUseProgram(ortho_shader);

    shader_set_mat4(ortho_shader, "view", &ortho_views[orientation]);
    // shader_set_mat4(ortho_shader, "view", &isometric_view);

	// Projection matrix
	mat4 projection = {0};
    float aspect = (float)window_width / (float)window_height;

    float left = -zoom;
    float right = zoom;

    float ortho_height = zoom / aspect;
    float bottom = -ortho_height;
    float top = ortho_height;

    glm_ortho(left, right, bottom, top, 0.1f, 100.0f, projection);
	
	shader_set_mat4(ortho_shader, "projection", &projection);

	shader_set_int(ortho_shader, "texture1", 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (i32 z = 0; z < level->depth; z++) {
		for (i32 y = 0; y < level->height; y++) {
			for (i32 x = 0; x < level->width; x++) {
				size_t tile_index = level_get_tile_index(level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }

				tile_t tile = res_pack->tiles[tile_index];

				mesh_t mesh = res_pack->meshes[tile.mesh_index];

                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
				// glm_translate(model, (vec3){x * 2.0, y * 2.0, z * 2.0});
				glm_rotate(model, glm_rad(tile.rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
				shader_set_mat4(ortho_shader, "model", &model);
				
				render_mesh(res_pack, &mesh, tile.texture_index);
			}
		}
	}

	// end_frame_buffer(res_pack);
}