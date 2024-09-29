#include "editor.h"

static mat4 top_view = {0};
static mat4 side_view = {0};
static mat4 *current_view = NULL;

static u32 shader_program = 0;

static f32 zoom = 10.0f;

void editor_init(void) {
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

    current_view = &top_view;

    shader_program = create_shader_program("res/shaders/vertex_editor.glsl", "res/shaders/fragment_editor.glsl");
}

void editor_update(void) {
    if (input_key_pressed(SDL_SCANCODE_1)) {
        current_view = &top_view;
    }
    if (input_key_pressed(SDL_SCANCODE_2)) {
        current_view = &side_view;
    }
}

void editor_render(res_pack_t *res_pack, level_t *level) {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    glUseProgram(shader_program);

    shader_set_mat4(shader_program, "view", current_view);

	// Projection matrix
	mat4 projection;
    float aspect = (float)window_width / (float)window_height;

    float left = -zoom;
    float right = zoom;

    float ortho_height = zoom / aspect;
    float bottom = -ortho_height;
    float top = ortho_height;

    glm_ortho(left, right, bottom, top, 0.1f, 100.0f, projection);
	
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

void editor_input(SDL_Event event) {
    if (event.type == SDL_MOUSEWHEEL) {
		zoom -= (float)event.wheel.y;
	}
}