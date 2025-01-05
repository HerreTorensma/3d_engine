#include "engine/res.h"
#include "gamedefs.h"

void load_game(res_pack_t *res_pack) {
	res_init(res_pack);

    res_pack->render_width = 640;
	res_pack->render_height = 360;

	// res_pack->sky_color = (color_t){58, 49, 41, 255};
	// res_pack->sky_color = (color_t){0, 20, 150, 255};
	// res_pack->sky_color = (color_t){149, 177, 225, 255};
	res_pack->sky_color = (color_t){0, 0, 0, 255};

	// // res_pack.sky_color = (color_t){2, 9, 23, 255};
	// // res_pack.sky_color = (color_t){0, 0, 0, 255};
	// res_pack->fog_color = (color_t){58, 49, 41, 255};
	res_pack->fog_color = (color_t){58, 49, 41, 0};

	// res_pack->sky_color = COLOR_BLACK;
	// res_pack->fog_color = COLOR_BLACK;

	res_pack->editor_color = (color_t){50, 50, 50, 255};

	res_add_texture(res_pack, TEX_BRICKS, load_tga("res/images/bricks.tga", true));
	res_add_texture(res_pack, TEX_DIRT, load_tga("res/images/dirt.tga", true));
	res_add_texture(res_pack, TEX_GRASS, load_tga("res/images/grass.tga", false));
	res_add_texture(res_pack, TEX_IRON_BARS, load_tga("res/images/iron_bars.tga", true));
	res_add_texture(res_pack, TEX_PLANKS, load_tga("res/images/planks.tga", false));
	res_add_texture(res_pack, TEX_SAND, load_tga("res/images/sand.tga", false));
	res_add_texture(res_pack, TEX_ROCK, load_tga("res/images/rock.tga", false));
	
	res_add_texture(res_pack, TEX_TREE, load_tga("res/images/tree.tga", true));
	res_add_texture(res_pack, TEX_BARREL, load_tga("res/images/barrel.tga", true));
	res_add_texture(res_pack, TEX_CHARACTER, load_tga("res/images/character.tga", true));

	res_add_texture(res_pack, TEX_FONT, load_tga("res/images/font.tga", false));
	res_add_texture(res_pack, TEX_CROSSHAIR, load_tga("res/images/crosshair.tga", true));
	
	res_add_texture(res_pack, TEX_BUTTON, load_tga("res/images/button_sheet.tga", false));
	res_add_texture(res_pack, TEX_BUTTON_PRESSED, load_tga("res/images/button_pressed_sheet.tga", false));
	res_pack->button_tex_index = TEX_BUTTON;
	res_pack->button_pressed_tex_index = TEX_BUTTON_PRESSED;
	res_pack->gui_tile_size = 8;

	res_add_texture(res_pack, TEX_FRAME, load_tga("res/images/frame.tga", false));
	res_add_texture(res_pack, TEX_BUTTON_SELECTED_INDICATOR, load_tga("res/images/button_selected_indicator.tga", false));
    res_pack->button_pressed_indicator_tex_index = TEX_BUTTON_SELECTED_INDICATOR;

	res_add_texture(res_pack, TEX_GLOCK_THUMBNAIL, load_tga("res/images/glock.tga", false));
	res_add_texture(res_pack, TEX_CIGARETTE, load_tga("res/images/cigarette.tga", false));
	
	res_add_item(res_pack, ITEM_GLOCK, (item_t){
		.stackable = false,
		.thumbnail_index = TEX_GLOCK_THUMBNAIL,
		.stats_type = STATS_GUN,
		.stats.gun = (gun_stats_t){
			.automatic = false,
			.fire_delay = 0.2f,
			.mag_size = 17,
			.reload_time = 2.0f,
		},
	});
	res_add_item(res_pack, ITEM_CIGARETTE, (item_t){
		.stackable = true,
		.thumbnail_index = TEX_CIGARETTE,
		.stats_type = STATS_NONE,
	});

	multi_box_collider_t wall_corner_collider = {0};
	wall_corner_collider.boxes[0] = (box_t){
		.min_x = -0.5f,
		.max_x = 0.1f,
		.min_y = -0.5f,
		.max_y = 0.5f,
		.min_z = -0.1f,
		.max_z = 0.1f,
	};
	wall_corner_collider.boxes[1] = (box_t){
		.min_x = -0.1f,
		.max_x = 0.1f,
		.min_y = -0.5f,
		.max_y = 0.5f,
		.min_z = -0.5f,
		.max_z = -0.1f,
	};
	wall_corner_collider.boxes_len = 2;

	res_add_mesh(res_pack, MESH_CUBE, load_mesh("res/meshes/cube.mesh"), NULL);
	res_add_mesh(res_pack, MESH_FLOOR, load_mesh("res/meshes/floor.mesh"), NULL);
	res_add_mesh(res_pack, MESH_SLAB, load_mesh("res/meshes/slab.mesh"), NULL);
	res_add_mesh(res_pack, MESH_SLOPE, load_mesh("res/meshes/slope.mesh"), NULL);
	res_add_mesh(res_pack, MESH_PYRAMID, load_mesh("res/meshes/pyramid.mesh"), NULL);
	res_add_mesh(res_pack, MESH_CORNER, load_mesh("res/meshes/corner.mesh"), NULL);
	res_add_mesh(res_pack, MESH_MONKEY, load_mesh("res/meshes/monkey.mesh"), NULL);
	res_add_mesh(res_pack, MESH_MUSHROOM, load_mesh("res/meshes/mushroom.mesh"), NULL);
	res_add_mesh(res_pack, MESH_WALL, load_mesh("res/meshes/wall.mesh"), NULL);
	res_add_mesh(res_pack, MESH_WALL_CORNER, load_mesh("res/meshes/wall_corner.mesh"), &wall_corner_collider);
	res_add_mesh(res_pack, MESH_LOWER_SLAB, load_mesh("res/meshes/lower_slab.mesh"), NULL);

	// print_box(&res_pack->meshes[MESH_LOWER_SLAB].collider.boxes[0]);

	res_add_sound(res_pack, SOUND_JUMP, (sound_t){Mix_LoadWAV("res/sounds/jump.wav")});
}