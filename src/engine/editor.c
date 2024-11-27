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

static i32 selected_y_level = 0;
static bool show_stack = true;

i32 start_tile_x = 0;
i32 start_tile_z = 0;

typedef enum {
	DRAW_MODE_DRAG,
	DRAW_MODE_STACK,
	DRAW_MODE_RECT,
} draw_mode_t;

draw_mode_t draw_mode = DRAW_MODE_DRAG;

const rect_t drawable_area = {
	.x = 32,
	.y = 32,
	.w = 576,
	.h = 296,
};

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

static void get_mouse_tile_pos(i32 *x, i32 *y) {
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

static void clamp_tile_coords(grid_t *grid, i32 *tile_x, i32 *tile_z) {
	if (*tile_x < 0) {
		*tile_x = 0;
	}
	if (*tile_x >= grid->width) {
		*tile_x = grid->width - 1;
	}

	if (*tile_z < 0) {
		*tile_z = 0;
	}
	if (*tile_z >= grid->depth) {
		*tile_z = grid->depth - 1;
	}
}

void editor_update(res_pack_t *res_pack, grid_t *grid) {
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

	if (input_key_pressed(SDL_SCANCODE_MINUS)) {
		if (selected_y_level > 0) {
			selected_y_level--;
		}
	}

	if (input_key_pressed(SDL_SCANCODE_EQUALS)) {
		if (selected_y_level < grid->height - 1) {
			selected_y_level++;
		}
	}

	compute_projection();

	// Just create it every frame based on the mesh and texture
	tile_t selected_tile = {
		.occupied = true,
		.mesh_index = selected_mesh_index,
		.texture_index = selected_texture_index,
	};

	i32 mouse_x, mouse_y;
	get_mouse_pos(&mouse_x, &mouse_y);
	mouse_x /= (window_width / res_pack->render_width);
    mouse_y /= (window_height / res_pack->render_height);
	if (mouse_x < drawable_area.x || mouse_y < drawable_area.y || mouse_x > drawable_area.x + drawable_area.w || mouse_y > drawable_area.y + drawable_area.h) {
		return;
	}

	if (draw_mode == DRAW_MODE_DRAG) {
		i32 tile_x, tile_z;
		get_mouse_tile_pos(&tile_x, &tile_z);
		if (tile_x < 0 || tile_x >= grid->width) {
			return;
		}
		if (tile_z < 0 || tile_z >= grid->depth) {
			return;
		}

		if (input_mouse_button_held(SDL_BUTTON_LEFT)) {
			grid_set_cell(grid, selected_tile, tile_x, selected_y_level, tile_z);
		}

		if (input_mouse_button_held(SDL_BUTTON_RIGHT)) {
			tile_t empty_tile = {0};
			grid_set_cell(grid, empty_tile, tile_x, selected_y_level, tile_z);
		}
	} else if (draw_mode == DRAW_MODE_RECT) {
		if (input_mouse_button_pressed(SDL_BUTTON_LEFT)) {
			get_mouse_tile_pos(&start_tile_x, &start_tile_z);
		}

		if (input_mouse_button_released(SDL_BUTTON_LEFT)) {
			i32 end_tile_x, end_tile_z;
			get_mouse_tile_pos(&end_tile_x, &end_tile_z);
			end_tile_x++;
			end_tile_z++;

			clamp_tile_coords(grid, &start_tile_x, &start_tile_z);
			clamp_tile_coords(grid, &end_tile_x, &end_tile_z);

			i32 left_x = start_tile_x;
			i32 right_x = end_tile_x;
			if (start_tile_x > end_tile_x) {
				left_x = end_tile_x;
				right_x = start_tile_x;
			}
			
			i32 top_y = start_tile_z;
			i32 bottom_y = end_tile_z;
			if (start_tile_z > end_tile_z) {
				top_y = end_tile_z;
				bottom_y = start_tile_z;
			}

			for (i32 y = top_y; y < bottom_y; y++) {
				for (i32 x = left_x; x < right_x; x++) {
					grid_set_cell(grid, selected_tile, x, selected_y_level, y);
				}
			}
		}
	} else if (draw_mode == DRAW_MODE_STACK) {
		i32 tile_x, tile_z;
		get_mouse_tile_pos(&tile_x, &tile_z);
		if (tile_x < 0 || tile_x >= grid->width) {
			return;
		}
		if (tile_z < 0 || tile_z >= grid->depth) {
			return;
		}

		if (input_mouse_button_pressed(SDL_BUTTON_LEFT)) {
			for (i32 i = grid->height - 2; i >= selected_y_level; i--) {
				if (grid_get_cell(grid, tile_x, i, tile_z).occupied) {
					grid_set_cell(grid, selected_tile, tile_x, i + 1, tile_z);
					break;
				}

				if (i == selected_y_level) {
					grid_set_cell(grid, selected_tile, tile_x, i, tile_z);
				}
			}
		}

		if (input_mouse_button_pressed(SDL_BUTTON_RIGHT)) {
			for (i32 i = grid->height - 2; i >= selected_y_level; i--) {
				if (grid_get_cell(grid, tile_x, i, tile_z).occupied) {
					tile_t empty_tile = {0};
					grid_set_cell(grid, empty_tile, tile_x, i, tile_z);
					break;
				}
			}
		}
	}
}

static bool mesh_button(res_pack_t *res_pack, size_t mesh_index, size_t texture_index, rect_t rect) {
	bool released = false;
	released = gui_button(res_pack, "", rect);

	render_mesh_isometric(res_pack, res_pack->meshes[mesh_index], texture_index, rect.x * res_pack->gui_tile_size + (rect.w / 2) * res_pack->gui_tile_size, rect.y * res_pack->gui_tile_size + (rect.h / 2) * res_pack->gui_tile_size, 8.0f);

	if (mesh_index == selected_mesh_index) {
		render_image(res_pack, res_pack->button_pressed_indicator_tex_index, rect.x * res_pack->gui_tile_size + rect.w * res_pack->gui_tile_size, rect.y * res_pack->gui_tile_size + 12, COLOR_WHITE);
	}

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

	if (texture_index == selected_texture_index) {
		render_image(res_pack, res_pack->button_pressed_indicator_tex_index, tile_rect.x * res_pack->gui_tile_size - 8, tile_rect.y * res_pack->gui_tile_size + 12, COLOR_WHITE);
	}

	return released;
}

void editor_render(res_pack_t *res_pack, grid_t *grid, index_t mesh_amount, index_t tex_amount) {
	// render_start_frame_buffer(res_pack);

	if (show_stack) {
		// render_grid_ortho(res_pack, grid, orientation, zoom, &projection, selected_y_level, grid->height - 1);
		render_grid_ortho(res_pack, grid, orientation, zoom, &projection, 0, grid->height - 1, true);
	} else {
		render_grid_ortho(res_pack, grid, orientation, zoom, &projection, selected_y_level, selected_y_level, false);
	}

	if (gui_button(res_pack, "SAVE", (rect_t){0, 0, 4, 2})) {
		grid_save(grid, "test2.grid");
		debug_log("Saved grid test.grid\n");
	}
	
	// gui_text_edit(res_pack, idk_buffer, 32, (rect_t){4, 4, 4, 4});

	for (index_t i = 2; i < mesh_amount; i++) {
		if (mesh_button(res_pack, i, 1, (rect_t){0, -4 + i * 4, 4, 4})) {
			selected_mesh_index = i;
			debug_log("Selected mesh %d\n", i);
		}
	}

	for (index_t i = 1; i < tex_amount; i++) {
		// Right edge of the screen
		if (texture_button(res_pack, i, (rect_t){76, i * 4, 4, 4})) {
			selected_texture_index = i;
			debug_log("Selected texture %d\n", i);
		}
	}

	char y_level_buffer[8];
	sprintf(y_level_buffer, "Y: %d [-/+]", selected_y_level);
	gui_print(res_pack, &res_pack->font, y_level_buffer, 32, 32, COLOR_WHITE);

	if (show_stack) {
		if (gui_button(res_pack, "VIEW MODE: STACK", (rect_t){4, 0, 10, 2})) {
			show_stack = false;
			return;
		}
	} else {
		if (gui_button(res_pack, "VIEW MODE: LAYER", (rect_t){4, 0, 10, 2})) {
			show_stack = true;
			return;
		}
	}

	if (draw_mode == DRAW_MODE_DRAG) {
		if (gui_button(res_pack, "DRAW MODE: DRAG", (rect_t){0, 2, 10, 2})) {
			draw_mode = DRAW_MODE_STACK;
			return;
		}
	}

	if (draw_mode == DRAW_MODE_STACK) {
		if (gui_button(res_pack, "DRAW MODE: STACK", (rect_t){0, 2, 10, 2})) {
			draw_mode = DRAW_MODE_RECT;
			return;
		}
	}

	if (draw_mode == DRAW_MODE_RECT) {
		if (gui_button(res_pack, "DRAW MODE: RECT", (rect_t){0, 2, 10, 2})) {
			draw_mode = DRAW_MODE_DRAG;
			return;
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