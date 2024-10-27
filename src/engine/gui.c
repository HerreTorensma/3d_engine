#include "gui.h"

void font_init(font_t *font, res_pack_t *res_pack, size_t texture_index) {
    texture_t texture = res_pack->textures[texture_index];

    font->texture_index = texture_index;
    font->horizontal_spacing = 1;

    u8 current_char = ' ';
    u32 last_start = 0;
    for (size_t x = 0; x < texture.width; x++) {
        size_t index = x * 4;

        u8 b = texture.pixels[index + 0];
        u8 g = texture.pixels[index + 1];
        u8 r = texture.pixels[index + 2];

        if (r == 255 && g == 0 && b == 0) {
            font->rects[current_char] = (rect_t){
                .x = last_start,
                .y = 0,
                .w = x - last_start,
                .h = texture.height,
            };

            current_char++;

            last_start = x + 1;
        }
    }

    // for (u8 i = ' '; i < 127; i++) {
    //     printf("%c: %d\n", i, font->offsets[i]);
    // }
}

void gui_print(res_pack_t *res_pack, font_t *font, const char text[], i32 x, i32 y) {
    i32 index = 0;
    i32 current_x = x;
    while (text[index] != '\0') {
        vec2 scale = {1.0f, 1.0f};
        render_image_ex(res_pack, font->texture_index, PIVOT_TOP_LEFT, font->rects[text[index]], current_x, y, 0.0f, scale);
        
        current_x += font->rects[text[index]].w + font->horizontal_spacing;
        
        index++;
    }
}

// TODO: tiny tilemap system for buttons
bool gui_button(res_pack_t *res_pack, const char text[], i32 x, i32 y) {
    size_t tex_index = res_pack->button_tex_index;

    i32 mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x /= (window_width / res_pack->render_width);
    mouse_y /= (window_height / res_pack->render_height);

    rect_t rect = {
        .x = x,
        .y = y,
        .w = res_pack->textures[res_pack->button_tex_index].width,
        .h = res_pack->textures[res_pack->button_tex_index].height,
    };

    bool released;
	if (mouse_x >= rect.x && mouse_x < rect.x + rect.w && mouse_y >= rect.y && mouse_y < rect.y + rect.h) {
        if (input_mouse_button_held(SDL_BUTTON_LEFT)) {
			tex_index = res_pack->button_pressed_tex_index;
		}

		if (input_mouse_button_released(SDL_BUTTON_LEFT)) {
			released = true;
		}
    }

    render_image(res_pack, PIVOT_TOP_LEFT, tex_index, x, y);
    gui_print(res_pack, &res_pack->font, text, x+2, y+4);

    return released;
}