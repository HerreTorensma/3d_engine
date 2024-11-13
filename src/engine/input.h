/*
This header and source file contains an input wrapper around SDL2.
TODO: support for key rebinding
*/
#pragma once

#include "global.h"

typedef struct input {
	u8 last_key_state[512];
	u8 key_state[512];

	u32 last_mouse_state;
	u32 mouse_state;
} input_t;

void input_update();

bool input_key_pressed(u32 scancode);

bool input_key_held(u32 scancode);

bool input_key_released(u32 scancode);

bool input_mouse_button_pressed(u32 button);

bool input_mouse_button_released(u32 button);

bool input_mouse_button_held(u32 button);