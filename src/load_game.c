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

	res_add_texture(res_pack, TEX_BRICKS, "res/images/bricks.tga");
	res_add_texture(res_pack, TEX_DIRT, "res/images/dirt.tga");
	res_add_texture(res_pack, TEX_GRASS, "res/images/grass.tga");
	res_add_texture(res_pack, TEX_IRON_BARS, "res/images/iron_bars.tga");
	res_add_texture(res_pack, TEX_PLANKS, "res/images/planks.tga");
	res_add_texture(res_pack, TEX_SAND, "res/images/sand.tga");
	res_add_texture(res_pack, TEX_ROCK, "res/images/rock.tga");
	
	res_add_texture(res_pack, TEX_TREE, "res/images/tree.tga");
	res_add_texture(res_pack, TEX_BARREL, "res/images/barrel.tga");
	res_add_texture(res_pack, TEX_CHARACTER, "res/images/character.tga");

	res_add_texture(res_pack, TEX_FONT, "res/images/font.tga");
	res_add_texture(res_pack, TEX_CROSSHAIR, "res/images/crosshair.tga");
	
	res_add_texture(res_pack, TEX_BUTTON, "res/images/button_sheet.tga");
	res_add_texture(res_pack, TEX_BUTTON_PRESSED, "res/images/button_pressed_sheet.tga");
	res_pack->button_tex_index = TEX_BUTTON;
	res_pack->button_pressed_tex_index = TEX_BUTTON_PRESSED;
	res_pack->gui_tile_size = 8;

	res_add_texture(res_pack, TEX_FRAME, "res/images/frame.tga");
	res_add_texture(res_pack, TEX_BUTTON_SELECTED_INDICATOR, "res/images/button_selected_indicator.tga");
    res_pack->button_pressed_indicator_tex_index = TEX_BUTTON_SELECTED_INDICATOR;

	res_add_texture(res_pack, TEX_GLOCK_THUMBNAIL, "res/images/glock.tga");
	res_add_texture(res_pack, TEX_CIGARETTE, "res/images/cigarette.tga");
	res_add_texture(res_pack, TEX_GLOCK_FRAME, "res/images/glock_frame.tga");
	res_add_texture(res_pack, TEX_CIGARETTE_FRAME, "res/images/cigarette_frame.tga");
	res_add_texture(res_pack, TEX_BULLET, "res/images/bullet.tga");
	
	res_add_item(res_pack, ITEM_GLOCK, (item_t){
		.name = "Glock",
		.desc = "A small handgun",

		.stackable = false,
		.thumbnail_index = TEX_GLOCK_THUMBNAIL,
		.stats_type = STATS_GUN,
		.stats.gun = (gun_stats_t){
			.image_index = TEX_GLOCK_FRAME,
			.automatic = false,
			.fire_delay = 0.2f,
			.mag_size = 17,
			.reload_time = 2.0f,
		},
	});
	res_add_item(res_pack, ITEM_CIGARETTE, (item_t){
		.name = "Cigarette",
		.desc = "A nice smokable cig that you could smoke or litter. 10mg nicotine",

		.stackable = true,
		.image_index = TEX_CIGARETTE_FRAME,
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

	res_add_mesh(res_pack, MESH_CUBE, "res/meshes/cube.mesh", NULL);
	res_add_mesh(res_pack, MESH_FLOOR, "res/meshes/floor.mesh", NULL);
	res_add_mesh(res_pack, MESH_SLAB, "res/meshes/slab.mesh", NULL);
	res_add_mesh(res_pack, MESH_SLOPE, "res/meshes/slope.mesh", NULL);
	res_add_mesh(res_pack, MESH_PYRAMID, "res/meshes/pyramid.mesh", NULL);
	res_add_mesh(res_pack, MESH_CORNER, "res/meshes/corner.mesh", NULL);
	res_add_mesh(res_pack, MESH_MONKEY, "res/meshes/monkey.mesh", NULL);
	res_add_mesh(res_pack, MESH_MUSHROOM, "res/meshes/mushroom.mesh", NULL);
	res_add_mesh(res_pack, MESH_WALL, "res/meshes/wall.mesh", NULL);
	res_add_mesh(res_pack, MESH_WALL_CORNER, "res/meshes/wall_corner.mesh", &wall_corner_collider);
	res_add_mesh(res_pack, MESH_LOWER_SLAB, "res/meshes/lower_slab.mesh", NULL);

	// print_box(&res_pack->meshes[MESH_LOWER_SLAB].collider.boxes[0]);

	res_add_sound(res_pack, SOUND_JUMP, (sound_t){Mix_LoadWAV("res/sounds/jump.wav")});
}