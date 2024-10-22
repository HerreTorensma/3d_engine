#include "editor.h"

static enum ortho_view orientation = 0;

static f32 zoom = 10.0f;

i32 offset_x = 0;
i32 offset_y = 0;

mat4 top_view = {0};

void editor_init(void) {
    {
		vec3 camera_pos = {0.0f, 10.0f, 0.0f};  // Camera position (above looking down)
		vec3 target_pos = {0.0f, 0.0f, 0.0f};   // Looking at the origin
		vec3 up_vector  = {0.0f, 0.0f, -1.0f};  // Up vector (in Blender's coordinate system)
		glm_lookat(camera_pos, target_pos, up_vector, top_view);
	}
}

void editor_update(level_t *level) {
    if (input_key_pressed(SDL_SCANCODE_1)) {
		orientation = ORTHO_TOP;
    }
    if (input_key_pressed(SDL_SCANCODE_2)) {
		orientation = ORTHO_FRONT;
    }

	if (input_mouse_button_pressed(SDL_BUTTON_LEFT)) {
		printf("mouse pressed\n");
		int x, y;
		SDL_GetMouseState(&x, &y);
		printf("x: %d, y: %d\n", x, y);

		// float aspect = window_width / window_height;
		// printf("offset: %d %d", );
		float aspect = (float)window_width / (float)window_height;
		printf("aspect: %f\n", aspect);

		float left = -zoom;
		float right = zoom;

		float ortho_height = zoom / aspect;
		float bottom = -ortho_height;
		float top = ortho_height;

		// printf("left: %d\n", left);
		// printf("thing: %f\n", (window_width / 4) * zoom);
		// printf("thing: %f\n", zoom / aspect);

		{
			float mouse_ndc_x = (2.0f * x) / window_width - 1.0f;
			float mouse_ndc_y = 1.0f - (2.0f * y) / window_height;
			printf("mouse x: %f, y: %f\n", mouse_ndc_x, mouse_ndc_y);
			
			vec4 ndc = { (2.0f * x) / window_width - 1.0f, 
             1.0f - (2.0f * y) / window_height, 
             0.0f, 1.0f };  // Near plane depth

			// Invert the view-projection matrix
			mat4 view_projection_inverse;

			mat4 projection = {0};
			float aspect = (float)window_width / (float)window_height;

			float left = -zoom;
			float right = zoom;

			float ortho_height = zoom / aspect;
			float bottom = -ortho_height;
			float top = ortho_height;

			glm_ortho(left, right, bottom, top, 0.1f, 100.0f, projection);
			// glm_ortho(left, right, bottom, top, -1.0f, 1.0f, projection);

			mat4 view_projection = {0};
			glm_mat4_mul(projection, top_view, view_projection);
			// glm_mat4_inv(projection, view_projection_inverse);
			// glm_mat4_inv(view_projection, view_projection_inverse);
			glm_mat4_inv(projection, view_projection_inverse);

			// Unproject to get world coordinates
			vec4 world_pos;
			glm_mat4_mulv(view_projection_inverse, ndc, world_pos);
			// glm_mat4_mulv(projection, ndc, world_pos);

			// Perform perspective divide
			// world_pos[0] /= world_pos[3];
			// world_pos[1] /= world_pos[3];

			// Map world coordinates to tile indices
			// int tileX = (int)(world_pos[0] + 0.5f);
			// int tileY = (int)(world_pos[1] + 0.5f);
			
			// int tileX = (int)(floorf(world_pos[0]) + 0.5f);
			// int tileY = (int)(floorf(world_pos[1]) + 0.5f);

			// int tileX, tileY;

			// if (world_pos[0] > 0) {
			// 	tileX = (int)(world_pos[0] + 0.5f);
			// } else {
			// 	tileX = (int)(world_pos[0] - 0.5f);
			// }
			// if (world_pos[1] > 0) {
			// 	tileX = (int)(world_pos[1] + 0.5f);
			// } else {
			// 	tileX = (int)(world_pos[1] - 0.5f);
			// }
			int tileX = (int)(roundf(world_pos[0]));
			int tileY = -(int)(roundf(world_pos[1]));

			printf("tile x: %d, y: %d\n", tileX, tileY);

			// Go down from 7
			for (int i = level->depth - 2; i >= 0; i--) {
				// if (level_get_tile_index(level, tileX, tileY, i) != 0) {
				// 	level_set_tile_index(level, 1, tileX, tileY, i + 1);
				// 	printf("bello\n");
				// 	break;
				// }

				if (level_get_tile_index(level, tileX, i, tileY) != 0) {
					level_set_tile_index(level, 1, tileX, i + 1, tileY);
					printf("bello\n");
					break;
				}
			}
		}

	}
}

void editor_render(res_pack_t *res_pack, level_t *level) {
	render_level_ortho(res_pack, level, orientation, zoom);
}

void editor_input(SDL_Event event) {
    if (event.type == SDL_MOUSEWHEEL) {
		zoom -= (float)event.wheel.y;
	}

	if (zoom < 1.0f) {
		zoom = 1.0f;
	}
}