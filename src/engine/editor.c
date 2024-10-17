#include "editor.h"

static enum ortho_view orientation = 0;

static f32 zoom = 10.0f;

void editor_init(void) {
    
}

void editor_update(void) {
    if (input_key_pressed(SDL_SCANCODE_1)) {
		orientation = ORTHO_TOP;
    }
    if (input_key_pressed(SDL_SCANCODE_2)) {
		orientation = ORTHO_FRONT;
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