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

void editor_update(level_t *level) {
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
			if (level_get_tile_index(level, tile_x, i, tile_y) != 0) {
				level_set_tile_index(level, 1, tile_x, i + 1, tile_y);
				break;
			}

			if (i == 0) {
				level_set_tile_index(level, 1, tile_x, i, tile_y);
			}
		}
	}

	if (input_mouse_button_pressed(SDL_BUTTON_RIGHT)) {
		for (int i = level->depth - 2; i >= 0; i--) {
			if (level_get_tile_index(level, tile_x, i, tile_y) != 0) {
				level_set_tile_index(level, 0, tile_x, i, tile_y);
				break;
			}
		}
	}
}

void editor_render(res_pack_t *res_pack, level_t *level) {
	render_level_ortho(res_pack, level, orientation, zoom, &projection);
}

void editor_input(SDL_Event event) {
    if (event.type == SDL_MOUSEWHEEL) {
		zoom -= (float)event.wheel.y;
	}

	if (zoom < 1.0f) {
		zoom = 1.0f;
	}
}