#include "input.h"

static input_t input = {0};

void input_update() {
	// Copy current keyboard state into last frame keyboard state
	memcpy(input.last_key_state, input.key_state, 255 * sizeof(u8));

	const u8 *state = SDL_GetKeyboardState(NULL);
	memcpy(input.key_state, state, 255 * sizeof(u8));

	// Same for the mouse
	input.last_mouse_state = input.mouse_state;
	
	i32 x, y;
	input.mouse_state = SDL_GetMouseState(&x, &y);

}

bool input_key_pressed(u32 scancode) {
	if (input.key_state[scancode] && !input.last_key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_key_held(u32 scancode) {
	if (input.key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_key_released(u32 scancode) {
	if (!input.key_state[scancode] && input.last_key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_mouse_button_pressed(u32 button) {
	if (input.mouse_state & SDL_BUTTON(button) && !(input.last_mouse_state & SDL_BUTTON(button))) {
		return true;
	}
	return false;
}

bool input_mouse_button_released(u32 button) {
	if (!(input.mouse_state & SDL_BUTTON(button)) && input.last_mouse_state & SDL_BUTTON(button)) {
		return true;
	}
	return false;
}

bool input_mouse_button_held(u32 button) {
	if (input.mouse_state & SDL_BUTTON(button)) {
		return true;
	}
	return false;
}