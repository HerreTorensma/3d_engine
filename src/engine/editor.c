#include "editor.h"

static float camera_speed = 0.2;

static enum ortho_view orientation = 0;

static f32 zoom = 10.0f;

i32 offset_x = 0;
i32 offset_y = 0;

mat4 top_view = {0};

mat4 projection = {0};

// vec3 pos = {2.0f, 0.0f, 0.0f};
vec2 pos = {2.0f, 0.0f};

static size_t selected_mesh_index = 2;
static size_t selected_texture_index = 1;

static char idk_buffer[32] = "test";

void editor_init(void) {
    {
		vec3 camera_pos = {0.0f, 10.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 0.0f, -1.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, top_view);
	}
}

static void compute_projection(void) {
	// Projection matrix
	float aspect = (float)window_width / (float)window_height;

	float left = -zoom + pos[0];
	float right = zoom + pos[0];

	float ortho_height = zoom / aspect;
	float bottom = -ortho_height + pos[1];
	float top = ortho_height + pos[1];

	glm_ortho(left, right, bottom, top, 0.1f, 100.0f, projection);
}

static void get_tile_pos(i32 *x, i32 *y) {
	float normalized_mouse_x = 0;
	float normalized_mouse_y = 0;
	get_normalized_mouse_pos(&normalized_mouse_x, &normalized_mouse_y);

	vec4 normalized_mouse_pos = {
		normalized_mouse_x,
		normalized_mouse_y,
		0.0f,
		1.0f,
	};

	mat4 projection_inverse = {0};
	glm_mat4_inv(projection, projection_inverse);
	vec4 world_pos = {0};
	glm_mat4_mulv(projection_inverse, normalized_mouse_pos, world_pos);

	*x = (int)(roundf(world_pos[0]));
	*y = -(int)(roundf(world_pos[1]));
}

void editor_update(grid_t *level) {
    if (input_key_pressed(SDL_SCANCODE_1)) {
		orientation = ORTHO_TOP;
    }
    if (input_key_pressed(SDL_SCANCODE_2)) {
		orientation = ORTHO_FRONT;
    }

	if (input_key_held(SDL_SCANCODE_D)) {
		pos[0] += camera_speed;
	}
	if (input_key_held(SDL_SCANCODE_A)) {
		pos[0] -= camera_speed;
	}
	if (input_key_held(SDL_SCANCODE_W)) {
		pos[1] += camera_speed;
	}
	if (input_key_held(SDL_SCANCODE_S)) {
		pos[1] -= camera_speed;
	}

	compute_projection();

	i32 tile_x, tile_y;
	get_tile_pos(&tile_x, &tile_y);
	if (tile_x < 0 || tile_x >= level->width) {
		return;
	}
	if (tile_y < 0 || tile_y >= level->height) {
		return;
	}

	if (input_mouse_button_pressed(SDL_BUTTON_LEFT)) {
		for (int i = level->depth - 2; i >= 0; i--) {
			if (grid_get_cell(level, tile_x, i, tile_y) != 0) {
				grid_set_cell(level, selected_mesh_index, tile_x, i + 1, tile_y);
				break;
			}

			if (i == 0) {
				grid_set_cell(level, selected_mesh_index, tile_x, i, tile_y);
			}
		}
	}

	if (input_mouse_button_pressed(SDL_BUTTON_RIGHT)) {
		for (int i = level->depth - 2; i >= 0; i--) {
			if (grid_get_cell(level, tile_x, i, tile_y) != 0) {
				grid_set_cell(level, 0, tile_x, i, tile_y);
				break;
			}
		}
	}
}

static bool mesh_button(res_pack_t *res_pack, size_t mesh_index, size_t texture_index, rect_t rect) {
	bool released = false;
	released = gui_button(res_pack, "", rect);

	render_mesh_isometric(res_pack, res_pack->meshes[mesh_index], texture_index, rect.x * res_pack->gui_tile_size + (rect.w / 2) * res_pack->gui_tile_size, rect.y * res_pack->gui_tile_size + (rect.h / 2) * res_pack->gui_tile_size, 8.0f);

	return released;
}

static bool texture_button(res_pack_t *res_pack, size_t texture_index, rect_t tile_rect) {
	bool released = false;
	released = gui_button(res_pack, "", tile_rect);

	// render_mesh_isometric(res_pack, res_pack->meshes[mesh_index], texture_index, rect.x * res_pack->gui_tile_size + (rect.w / 2) * res_pack->gui_tile_size, rect.y * res_pack->gui_tile_size + (rect.h / 2) * res_pack->gui_tile_size, 8.0f);
	// render_image(res_pack, PIVOT_TOP_LEFT, texture_index, rect.x, rect.y);
	// render_image_ex(res_pack, texture_index, PIVOT_CENTER, (rect_t){0, 0, 64, 64}, rect.x * res_pack->gui_tile_size, rect.y * res_pack->gui_tile_size, 0, (vec2){1.0f, 1.0f}, COLOR_WHITE);

	rect_t dst = {
		.x = tile_rect.x * res_pack->gui_tile_size + 2,
		.y = tile_rect.y * res_pack->gui_tile_size + 2,
		.w = tile_rect.w * res_pack->gui_tile_size - 5,
		.h = tile_rect.h * res_pack->gui_tile_size - 5,
	};

	render_image_rect(res_pack, texture_index, tex_get_default_src(res_pack, texture_index), dst, COLOR_WHITE);

	return released;
}

void editor_render(res_pack_t *res_pack, grid_t *level) {
	// render_start_frame_buffer(res_pack);

	render_grid_ortho(res_pack, level, orientation, zoom, &projection);

	if (gui_button(res_pack, "SAVE", (rect_t){0, 0, 4, 2})) {
		printf("SAVE\n");
	}
	
	gui_text_edit(res_pack, idk_buffer, 32, (rect_t){4, 4, 4, 4});

	for (i32 i = 0; i < 10; i++) {
		if (mesh_button(res_pack, i, 1, (rect_t){0, 2 + i * 4, 4, 4})) {
			// selected_mesh_index = i - 1;
			selected_mesh_index = i;
		}
	}

	for (i32 i = 0; i < 4; i++) {
		// Right edge of the screen
		if (texture_button(res_pack, i, (rect_t){76, 2 + i * 4, 4, 4})) {
			selected_texture_index = i;
			printf("selected a texture\n");
		}
	}

	// render_end_frame_buffer(res_pack);
}

void editor_input(SDL_Event event) {
    if (event.type == SDL_MOUSEWHEEL) {
		zoom -= (float)event.wheel.y;
	}

	if (zoom < 1.0f) {
		zoom = 1.0f;
	}
}