#include "render.h"

static u32 shader_program = 0;
static u32 basic_shader = 0;

static u32 fbo = 0;
static u32 fbo_tex = 0;

void render_init(void) {
    shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/fragment_tex.glsl");
    // shader_program = create_shader_program("res/shaders/vertex.glsl", "res/shaders/frag_checker.glsl");
    basic_shader = create_shader_program("res/shaders/vertex_default.glsl", "res/shaders/fragment_editor.glsl");
	
	// FBO and texture
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	u32 rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 640, 360);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenTextures(1, &fbo_tex);
	glBindTexture(GL_TEXTURE_2D, fbo_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 360, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Frame buffer not complete\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_level(res_pack_t *res_pack, level_t *level, camera_t *camera) {
	// Frame buffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 640, 360);

    // glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    // glClearColor(0.231f, 0.2f, 0.149f, 1.0f);
    // glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClearColor(0.227f, 0.192f, 0.161f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glDepthMask(GL_FALSE);
	// glDisable(GL_CULL_FACE);
	
    glUseProgram(shader_program);

	// View matrix
	mat4 view = {0};
	vec3 temp = {0};
	glm_vec3_add(camera->position, camera->front, temp);
	glm_lookat(camera->position, temp, camera->up, view);
	shader_set_mat4(shader_program, "view", &view);

	// Projection matrix
	mat4 projection = {0};
	// glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	glm_perspective(glm_rad(60.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, projection);
	
	shader_set_mat4(shader_program, "projection", &projection);

	shader_set_int(shader_program, "texture1", 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (i32 z = 0; z < level->depth; z++) {
		for (i32 y = 0; y < level->height; y++) {
			for (i32 x = 0; x < level->width; x++) {
				size_t tile_index = level_get_tile_index(level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }

				tile_t tile = res_pack->tiles[tile_index];
				if (tile.transparent) {
					continue;
				}

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
				shader_set_mat4(shader_program, "model", &model);
				
				// vec3 fog_color = {1.0f, 0.0f, 0.0f};
				// vec3 fog_color = {0.231f, 0.2f, 0.149f};
				// vec3 fog_color = {0.8f, 0.8f, 0.8f};
				vec3 fog_color = {0.227f, 0.192f, 0.161f};
				shader_set_vec3(shader_program, "fogColor", &fog_color);

				glBindVertexArray(mesh.vao);

				glBindTexture(GL_TEXTURE_2D, tile.texture_index);
				glActiveTexture(GL_TEXTURE0);

				glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
				
				glBindVertexArray(0);
			}
		}
	}

	for (i32 z = 0; z < level->depth; z++) {
		for (i32 y = 0; y < level->height; y++) {
			for (i32 x = 0; x < level->width; x++) {
				size_t tile_index = level_get_tile_index(level, x, y, z);
                if (tile_index == 0) {
                    continue;
                }

				tile_t tile = res_pack->tiles[tile_index];
				if (!tile.transparent) {
					continue;
				}

				if (tile.billboard) {
					shader_set_int(shader_program, "billboard", true);
				}


				mesh_t mesh = res_pack->meshes[tile.mesh_index];
                
                // Model matrix
				mat4 model;
				glm_mat4_identity(model);
				glm_translate(model, (vec3){x, y, z});
				glm_scale(model, (vec3){0.5f, 0.5f, 0.5f});
				glm_rotate(model, glm_rad(tile.rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
				glm_rotate(model, glm_rad(tile.rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
				
				// if (tile.billboard) {
				// 	glm_rotate(model, )
				// } else {
				// 	glm_rotate(model, glm_rad(tile.rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
				// 	glm_rotate(model, glm_rad(tile.rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
				// 	glm_rotate(model, glm_rad(tile.rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
				// }

				shader_set_mat4(shader_program, "model", &model);
				
				// vec3 fog_color = {1.0f, 0.0f, 0.0f};
				// vec3 fog_color = {0.231f, 0.2f, 0.149f};
				// vec3 fog_color = {0.8f, 0.8f, 0.8f};
				vec3 fog_color = {0.227f, 0.192f, 0.161f};
				shader_set_vec3(shader_program, "fogColor", &fog_color);

				glBindVertexArray(mesh.vao);

				glBindTexture(GL_TEXTURE_2D, tile.texture_index);
				glActiveTexture(GL_TEXTURE0);

				glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
				
				glBindVertexArray(0);
			}
		}
	}

	// More framebuffer stuff
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Bind default framebuffer
	glViewport(0, 0, window_width, window_height);  // Set viewport back to full resolution
	glDisable(GL_DEPTH_TEST);

	glUseProgram(basic_shader);

	glBindTexture(GL_TEXTURE_2D, fbo_tex);

	mesh_t mesh = res_pack->meshes[MESH_BIG_QUAD];
	glBindVertexArray(mesh.vao);

	glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}