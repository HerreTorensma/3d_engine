#include "gui.h"

static u32 current_id = 0;

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

void gui_print(res_pack_t *res_pack, font_t *font, const char text[], i32 x, i32 y, color_t color) {
    i32 index = 0;
    i32 current_x = x;
    while (text[index] != '\0') {
        vec2 scale = {1.0f, 1.0f};
        // render_image_ex(res_pack, font->texture_index, PIVOT_TOP_LEFT, font->rects[text[index]], current_x, y, 0.0f, scale, res_pack->font.color);
        rect_t dst = {
            .x = current_x,
            .y = y,
            .w = font->rects[text[index]].w,
            .h = font->rects[text[index]].h,
        };
        
        render_image_rect(res_pack, font->texture_index, font->rects[text[index]], dst, color);
        
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

static inline rect_t tile_to_global(res_pack_t *res_pack, rect_t tile_rect) {
    return (rect_t){
        .x = tile_rect.x * res_pack->gui_tile_size,
        .y = tile_rect.y * res_pack->gui_tile_size,
        .w = tile_rect.w * res_pack->gui_tile_size,
        .h = tile_rect.h * res_pack->gui_tile_size,
    };
}

bool gui_button(res_pack_t *res_pack, const char text[], rect_t tile_rect) {
    size_t tex_index = res_pack->button_tex_index;

    i32 mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x /= (window_width / res_pack->render_width);
    mouse_y /= (window_height / res_pack->render_height);

    rect_t global_rect = tile_to_global(res_pack, tile_rect);

    bool released = false;
	if (mouse_x >= global_rect.x && mouse_x < global_rect.x + global_rect.w && mouse_y >= global_rect.y && mouse_y < global_rect.y + global_rect.h) {
        if (input_mouse_button_held(SDL_BUTTON_LEFT)) {
			tex_index = res_pack->button_pressed_tex_index;
		}

		if (input_mouse_button_released(SDL_BUTTON_LEFT)) {
			released = true;
		}
    }

    for (u16 y = 0; y < tile_rect.h; y++) {
        for (u16 x = 0; x < tile_rect.w; x++) {
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
            if (x == tile_rect.w - 1) {
                src.x = 2 * res_pack->gui_tile_size;
            }
            if (y == tile_rect.h - 1) {
                src.y = 2 * res_pack->gui_tile_size;
            }

            rect_t dst = {
                .x = global_rect.x + x * res_pack->gui_tile_size,
                .y = global_rect.y + y * res_pack->gui_tile_size,
                .w = res_pack->gui_tile_size,
                .h = res_pack->gui_tile_size,
            };

            render_image_rect(res_pack, tex_index, src, dst, COLOR_WHITE);
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
    gui_print(res_pack, &res_pack->font, text, text_x, text_y, COLOR_BLACK);

    return released;
}

static inline sdl_scancode_to_char(SDL_Scancode scancode) {
    char character = '\0';

    switch (scancode) {
        case SDL_SCANCODE_A:
            character = 'A';
            break;
        case SDL_SCANCODE_B:
            character = 'B';
            break;
        case SDL_SCANCODE_C:
            character = 'C';
            break;
        case SDL_SCANCODE_D:
            character = 'D';
            break;
        case SDL_SCANCODE_E:
            character = 'E';
            break;
        case SDL_SCANCODE_F:
            character = 'F';
            break;
        case SDL_SCANCODE_G:
            character = 'G';
            break;
        case SDL_SCANCODE_H:
            character = 'H';
            break;
        case SDL_SCANCODE_I:
            character = 'I';
            break;
        case SDL_SCANCODE_J:
            character = 'J';
            break;
        case SDL_SCANCODE_K:
            character = 'K';
            break;
        case SDL_SCANCODE_L:
            character = 'L';
            break;
        case SDL_SCANCODE_M:
            character = 'M';
            break;
        case SDL_SCANCODE_N:
            character = 'N';
            break;
        case SDL_SCANCODE_O:
            character = 'O';
            break;
        case SDL_SCANCODE_P:
            character = 'P';
            break;
        case SDL_SCANCODE_Q:
            character = 'Q';
            break;
        case SDL_SCANCODE_R:
            character = 'R';
            break;
        case SDL_SCANCODE_S:
            character = 'S';
            break;
        case SDL_SCANCODE_T:
            character = 'T';
            break;
        case SDL_SCANCODE_U:
            character = 'U';
            break;
        case SDL_SCANCODE_V:
            character = 'V';
            break;
        case SDL_SCANCODE_W:
            character = 'W';
            break;
        case SDL_SCANCODE_X:
            character = 'X';
            break;
        case SDL_SCANCODE_Y:
            character = 'Y';
            break;
        case SDL_SCANCODE_Z:
            character = 'Z';
            break;
        
        case SDL_SCANCODE_1:
            character = '1';
            break;
        case SDL_SCANCODE_2:
            character = '2';
            break;
        case SDL_SCANCODE_3:
            character = '3';
            break;
        case SDL_SCANCODE_4:
            character = '4';
            break;
        case SDL_SCANCODE_5:
            character = '5';
            break;
        case SDL_SCANCODE_6:
            character = '6';
            break;
        case SDL_SCANCODE_7:
            character = '7';
            break;
        case SDL_SCANCODE_8:
            character = '8';
            break;
        case SDL_SCANCODE_9:
            character = '9';
            break;
        case SDL_SCANCODE_0:
            character = '0';
            break;

        case SDL_SCANCODE_SPACE:
            character = ' ';
            break;
    }

    return character;
}

void gui_text_edit(res_pack_t *res_pack, char text[], size_t buffer_size, rect_t tile_rect) {
    rect_t global_rect = tile_to_global(res_pack, tile_rect);

    gui_print(res_pack, &res_pack->font, text, global_rect.x, global_rect.y, COLOR_WHITE);

    size_t current_index = strlen(text);

    if (input_key_pressed(SDL_SCANCODE_BACKSPACE)) {
        text[current_index] = '\0';
        text[current_index - 1] = '\0';
        return;
    }

    if (current_index == buffer_size - 1) {
        return;
    }

    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        if (input_key_pressed(i)) {
            char character = sdl_scancode_to_char(i);
            text[current_index] = character;
            text[current_index + 1] = '\0';
        }
    }
}

void gui_id_reset(void) {
    current_id = 0;
}

u32 gui_id_gen(void) {
    current_id++;
    return current_id;
}