#pragma once

enum {
	MESH_CUBE = 1,
	MESH_SLAB,
	MESH_CORNER,
	MESH_FLOOR,
	MESH_PYRAMID,
	MESH_SLOPE,
	MESH_MONKEY,
	MESH_MUSHROOM,
	MESH_WALL,
	MESH_WALL_CORNER,
	MESH_LOWER_SLAB,
};

enum {
	TEX_BRICKS = 1,
	TEX_DIRT,
	TEX_GRASS,
	TEX_IRON_BARS,
    TEX_PLANKS,
	TEX_SAND,
	TEX_ROCK,

	TEX_TREE,
	TEX_BARREL,
	TEX_CHARACTER,

	TEX_FONT,
	TEX_CROSSHAIR,
	TEX_BUTTON,
	TEX_BUTTON_PRESSED,
	TEX_FRAME,
    TEX_BUTTON_SELECTED_INDICATOR,
};

enum {
	SOUND_JUMP = 1,
};

typedef enum entity_flag {
    HAS_SPRITE = 1,
    HAS_MESH = 2,
    HAS_COLLIDER = 3,
    HAS_PLAYER = 4,
} entity_flag_t;

typedef struct entity {
    bool is_valid;
	u64 flags;

	transform_c transform;
	sprite_c sprite;
	mesh_c mesh;
	collider_c collider;
	camera_c camera;
	player_controller_c player_controller;
	player_collider_c player_collider;
} entity_t;

enum {
    STATE_MAIN_MENU,
    STATE_GAMEPLAY,
};