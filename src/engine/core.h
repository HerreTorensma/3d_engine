/*
This header contains the most important structs used by the engine,
conveniently in one file to avoid cyclic imports.
*/
#pragma once

#include "global.h"

// Structs
typedef struct vertex {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
} vertex_t;

typedef struct box {
    float min_x;
    float max_x;
    
    float min_y;
    float max_y;

    float min_z;
    float max_z;
} box_t;

typedef struct collider {
    box_t boxes[4];
    index_t boxes_len;
} collider_t;

typedef struct mesh {
	vertex_t *vertices;
    u32 *indices;

    u32 vertex_count;
    u32 index_count;

    u32 vao;
    u32 vbo;
    u32 ebo;

    collider_t collider;
} mesh_t;

typedef struct texture {
    u16 width;
    u16 height;
    u8 *pixels;

	u32 id;
} texture_t;

typedef struct rect {
	i32 x;
	i32 y;
	i32 w;
	i32 h;
} rect_t;

typedef struct color {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} color_t;

typedef struct collision {
	bool hit;
	box_t global_box;
} collision_t;

typedef struct camera {
    vec3 position;
    vec3 front;
    vec3 up;

    f32 pitch;
    f32 yaw;
} camera_t;

typedef struct font {
    index_t texture_index;
    rect_t rects[256];
    u32 horizontal_spacing;
    i32 y_center;
} font_t;

typedef struct theme {
	u32 tile_size;
	
    font_t font;

	index_t button_tex_index;
	index_t button_pressed_tex_index;
    index_t button_pressed_indicator_tex_index;
} theme_t;

typedef struct tile {
    u8 occupied;
	u8 mesh_index;
	u8 texture_index;
} tile_t;

typedef struct grid {
    // Height refers to the vertical height, so not depth.
    // Also the 0 start at the "bottom", so y coord 0 will be eye level and 16 up in the sky.
	u32 width;
	u32 height;
	u32 depth;

    tile_t *map;
} grid_t;

inline tile_t grid_get_cell(grid_t *grid, u32 x, u32 y, u32 z) {
	return grid->map[z * grid->width * grid->height + y * grid->width + x];
}

inline void grid_set_cell(grid_t *grid, tile_t tile, u32 x, u32 y, u32 z) {
	grid->map[z * grid->width * grid->height + y * grid->width + x] = tile;
}

typedef struct sound {
    Mix_Chunk *chunk;
} sound_t;

typedef struct res_pack {
	color_t fog_color;
	color_t sky_color;
	color_t editor_color;

	u32 render_width;
	u32 render_height;

    mesh_t meshes[256];
	texture_t textures[256];

    sound_t sounds[256];

	index_t button_tex_index;
	index_t button_pressed_tex_index;
    index_t button_pressed_indicator_tex_index;
	u32 gui_tile_size;

	font_t font;
} res_pack_t;

typedef struct response {
    char text[512];
    i32 next_dialogue;
} response_t;

typedef struct dialogue {
    char text[512];
    response_t responses[8];
    i32 response_count;
} dialogue_t;

// Core definitions
#define COLOR_WHITE (color_t){255, 255, 255, 255}
#define COLOR_BLACK (color_t){0, 0, 0, 255}
#define COLOR_RED (color_t){255, 0, 0, 255}
#define COLOR_GREEN (color_t){0, 255, 0, 255}
#define COLOR_BLUE (color_t){0, 0, 0, 255}

// Core enums
enum screen_anchor {
    ANCHOR_CENTER,
    ANCHOR_TOP,
    ANCHOR_BOTTOM,
    ANCHOR_LEFT,
    ANCHOR_RIGHT,
    ANCHOR_TOPLEFT,
    ANCHOR_TOPRIGHT,
    ANCHOR_BOTTOMLEFT,
    ANCHOR_BOTTOMRIGHT,
};

enum ortho_view {
    ORTHO_TOP,
    ORTHO_BOTTOM,
    ORTHO_FRONT,
    ORTHO_BACK,
    ORTHO_LEFT,
    ORTHO_RIGHT,
};

// Core components required by the engine itself
typedef struct transform {
	vec3 position;
	vec3 rotation;
	vec3 scale;
} transform_t;

typedef transform_t transform_c;

typedef struct {
	index_t texture_index;
	bool billboard;

    float x_scale;
    float y_scale;
} sprite_c;

typedef struct {
    index_t mesh_index;
    index_t texture_index;
} mesh_c;

typedef collider_t collider_c;

// Continue your own enum at 3
enum {
    TRANSFORM_C = 0,
    SPRITE_C = 1,
    MESH_C = 2,
    COLLIDER_C = 3,
};