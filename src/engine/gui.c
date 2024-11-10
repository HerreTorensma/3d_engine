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

static i32 gui_get_text_width(res_pack_t *res_pack, font_t *font, const char text[]) {
    i32 index = 0;
    i32 current_x = 0;

    while (text[index] != '\0') {
        current_x += font->rects[text[index]].w + font->horizontal_spacing;
        index++;
    }

    return current_x;
}

bool gui_button(res_pack_t *res_pack, const char text[], rect_t rect) {
    size_t tex_index = res_pack->button_tex_index;

    i32 mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x /= (window_width / res_pack->render_width);
    mouse_y /= (window_height / res_pack->render_height);

    rect_t global_rect = {
        .x = rect.x * res_pack->gui_tile_size,
        .y = rect.y * res_pack->gui_tile_size,
        .w = rect.w * res_pack->gui_tile_size,
        .h = rect.h * res_pack->gui_tile_size,
    };

    bool released = false;
	if (mouse_x >= global_rect.x && mouse_x < global_rect.x + global_rect.w && mouse_y >= global_rect.y && mouse_y < global_rect.y + global_rect.h) {
        if (input_mouse_button_held(SDL_BUTTON_LEFT)) {
			tex_index = res_pack->button_pressed_tex_index;
		}

		if (input_mouse_button_released(SDL_BUTTON_LEFT)) {
			released = true;
		}
    }

    for (u16 y = 0; y < rect.h; y++) {
        for (u16 x = 0; x < rect.w; x++) {
            rect_t src = {
                .x = res_pack->gui_tile_size,
                .y = res_pack->gui_tile_size,
                .w = res_pack->gui_tile_size,
                .h = res_pack->gui_tile_size, 
            };

            if (x == 0) {
                src.x = 0;
            }
            if (y == 0) {
                src.y = 0;
            }
            if (x == rect.w - 1) {
                src.x = 2 * res_pack->gui_tile_size;
            }
            if (y == rect.h - 1) {
                src.y = 2 * res_pack->gui_tile_size;
            }

            render_image_ex(res_pack, tex_index, PIVOT_TOP_LEFT, src, global_rect.x + x * res_pack->gui_tile_size, global_rect.y + y * res_pack->gui_tile_size, 0, (vec2){1.0f, 1.0f});
        }
    }

    i32 text_width = gui_get_text_width(res_pack, &res_pack->font, text);

    i32 text_x = global_rect.x + (global_rect.w / 2) - (text_width / 2) - 1;
    // i32 text_y = global_rect.y + (global_rect.h / 2) + res_pack->font.y_center;
    i32 text_y = global_rect.y + (global_rect.h / 2) - (res_pack->textures[res_pack->font.texture_index].height / 2) - 1;

    // gui_print(res_pack, &res_pack->font, text, x+2, y+4);
    // gui_print(res_pack, &res_pack->font, text, global_rect.x, global_rect.y);
    // gui_print(res_pack, &res_pack->font, text, global_rect.x, text_y);
    // gui_print(res_pack, &res_pack->font, text, global_rect.x - text_width / 2, text_y);
    gui_print(res_pack, &res_pack->font, text, text_x, text_y);

    return released;
}