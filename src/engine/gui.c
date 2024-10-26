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