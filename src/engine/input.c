#include "input.h"

static input_t _input = {0};

void input_update() {
	// Copy current keyboard state into last frame keyboard state
	memcpy(_input.last_key_state, _input.key_state, 255 * sizeof(u8));

	const u8 *state = SDL_GetKeyboardState(NULL);
	memcpy(_input.key_state, state, 255 * sizeof(u8));

	// Same for the mouse
	_input.last_mouse_state = _input.mouse_state;
	
	i32 x, y;
	_input.mouse_state = SDL_GetMouseState(&x, &y);

}

bool input_key_pressed(SDL_Scancode scancode) {
	if (_input.key_state[scancode] && !_input.last_key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_key_held(SDL_Scancode scancode) {
	if (_input.key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_key_released(SDL_Scancode scancode) {
	if (!_input.key_state[scancode] && _input.last_key_state[scancode]) {
		return true;
	}
	return false;
}

bool input_mouse_button_pressed(u32 button) {
	if (_input.mouse_state & SDL_BUTTON(button) && !(_input.last_mouse_state & SDL_BUTTON(button))) {
		return true;
	}
	return false;
}

bool input_mouse_button_released(u32 button) {
	if (!(_input.mouse_state & SDL_BUTTON(button)) && _input.last_mouse_state & SDL_BUTTON(button)) {
		return true;
	}
	return false;
}

bool input_mouse_button_held(u32 button) {
	if (_input.mouse_state & SDL_BUTTON(button)) {
		return true;
	}
	return false;
}