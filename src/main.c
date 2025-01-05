#include "engine/global.h"
#include "engine/render.h"
#include "engine/util.h"
#include "engine/editor.h"
#include "engine/gui.h"
#include "engine/grid.h"
#include "engine/audio.h"
#include "engine/memory.h"
#include "engine/collision.h"
#include "engine/engine.h"

#include "entity.h"
#include "state.h"
#include "game.c"

state_t state = {0};

void input(SDL_Event event) {
	if (state.edit_mode) {
		editor_input(event);
	} else {
		game_input(event, ent_get(state.player_ent_index));
	}
}

int main(int argc, char *argv[]) {
	engine_init("My fuckass 'desert game' set in a currently unknown location", 1280, 720, 640, 360, input);

	load_game(&state.res_pack);

	SDL_SetRelativeMouseMode(SDL_TRUE);

    state.mode = STATE_GAMEPLAY;

    font_init(&state.res_pack.font, &state.res_pack, TEX_FONT);
	state.res_pack.font.y_center = -4;

	// grid_init(&grid, 512, 16, 512);
	// grid_load(&grid, "test.grid");
	grid_load(&state.grid, "test2.grid");

	ent_system_init(&state.ent_system);

    state.player_ent_index = spawn_player((transform_t){0});

	spawn_barrel((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 5.0f});
	spawn_tree((transform_t){.position[0] = 5.0f, .position[1] = 1.0f, .position[2] = 6.0f});

    render_init(&state.res_pack);
	editor_init();

	while (!window_should_close()) {
		engine_update();

		if (input_key_pressed(SDL_SCANCODE_E)) {
			state.edit_mode = !state.edit_mode;
			if (state.edit_mode) {
				SDL_SetRelativeMouseMode(SDL_FALSE);
			} else {
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
		}

		if (state.edit_mode) {
			editor_update(&state.res_pack, &state.grid);
		} else {
			game_update();
		}

		if (state.edit_mode) {
			editor_render(&state.res_pack, &state.grid, 13, 13);
		} else {
			game_render();
		}

		engine_update_end();
	}

	engine_quit();
}
