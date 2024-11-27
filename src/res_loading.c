#include "engine/res.h"

enum {
	MESH_CUBE = 2,
	MESH_SLAB,
	MESH_CORNER,
	MESH_FLOOR,
	MESH_PYRAMID,
	MESH_SLOPE,
	MESH_MONKEY,
	MESH_MUSHROOM,
	MESH_WALL,
	MESH_WALL_CORNER,
};

enum {
	TEX_BRICKS = 1,
	TEX_DIRT,
	TEX_COBBLE,
	TEX_GRASS,
	TEX_IRON_BARS,

	TEX_TREE,
	TEX_BIRCH,
	TEX_CROSSHAIR,
	TEX_WORMFISH,
	TEX_FONT,
	TEX_BUTTON,
	TEX_BUTTON_PRESSED,
	TEX_FRAME,
	TEX_RED,
    TEX_BUTTON_SELECTED_INDICATOR,
};

enum {
	SOUND_JUMP = 1,
};

void load_res(res_pack_t *res_pack) {
    res_pack->render_width = 640;
	res_pack->render_height = 360;

	res_pack->sky_color = (color_t){58, 49, 41, 255};
	// res_pack.sky_color = (color_t){2, 9, 23, 255};
	// res_pack.sky_color = (color_t){0, 0, 0, 255};
	res_pack->fog_color = (color_t){58, 49, 41, 255};
	res_pack->editor_color = (color_t){50, 50, 50, 255};

	res_add_texture(res_pack, TEX_BRICKS, load_tga("res/images/bricks.tga", true));
	res_add_texture(res_pack, TEX_DIRT, load_tga("res/images/dirt.tga", true));
	res_add_texture(res_pack, TEX_COBBLE, load_tga("res/images/cobble.tga", false));
	res_add_texture(res_pack, TEX_GRASS, load_tga("res/images/grass.tga", false));
	res_add_texture(res_pack, TEX_IRON_BARS, load_tga("res/images/iron_bars.tga", true));
	res_add_texture(res_pack, TEX_TREE, load_tga("res/images/tree.tga", true));
	res_add_texture(res_pack, TEX_BIRCH, load_tga("res/images/birch.tga", true));
	res_add_texture(res_pack, TEX_CROSSHAIR, load_tga("res/images/crosshair.tga", true));
	res_add_texture(res_pack, TEX_WORMFISH, load_tga("res/images/wormfish.tga", true));
	res_add_texture(res_pack, TEX_FONT, load_tga("res/images/font.tga", false));

	res_add_texture(res_pack, TEX_BUTTON, load_tga("res/images/button_sheet.tga", false));
	res_add_texture(res_pack, TEX_BUTTON_PRESSED, load_tga("res/images/button_pressed_sheet.tga", false));
	res_pack->button_tex_index = TEX_BUTTON;
	res_pack->button_pressed_tex_index = TEX_BUTTON_PRESSED;
	res_pack->gui_tile_size = 8;

	res_add_texture(res_pack, TEX_FRAME, load_tga("res/images/frame_new.tga", false));
	res_add_texture(res_pack, TEX_RED, load_tga("res/images/red.tga", false));
	res_add_texture(res_pack, TEX_BUTTON_SELECTED_INDICATOR, load_tga("res/images/button_selected_indicator.tga", false));
    res_pack->button_pressed_indicator_tex_index = TEX_BUTTON_SELECTED_INDICATOR;

	collider_t cube_collider = {0};
	cube_collider.boxes[0] = (box_t){
		.min_x = -0.5f,
		.max_x = 0.5f,
		.min_y = -0.5f,
		.max_y = 0.5f,
		.min_z = -0.5f,
		.max_z = 0.5f,
	};
	cube_collider.boxes_len = 1;

	collider_t floor_collider = {0};
	floor_collider.boxes[0] = (box_t){
		.min_x = -0.5f,
		.max_x = 0.5f,
		.min_y = -0.5f,
		.max_y = -0.5f,
		.min_z = -0.5f,
		.max_z = 0.5f,
	};
	floor_collider.boxes_len = 1;

	collider_t wall_collider = {0};
	wall_collider.boxes[0] = (box_t){
		.min_x = -0.5f,
		.max_x = 0.5f,
		.min_y = -0.5f,
		.max_y = 0.5f,
		.min_z = -0.1f,
		.max_z = 0.1f,
	};
	wall_collider.boxes[1] = floor_collider.boxes[0];
	wall_collider.boxes_len = 2;

	collider_t wall_corner_collider = {0};
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
	wall_corner_collider.boxes[2] = floor_collider.boxes[0];
	wall_corner_collider.boxes_len = 3;

	collider_t slab_collider = {0};
	slab_collider.boxes[0] = (box_t){
		.min_x = -0.5f,
		.max_x = 0.5f,
		.min_y = -0.5f,
		.max_y = 0.0f,
		.min_z = -0.5f,
		.max_z = 0.5f,
	};
	slab_collider.boxes_len = 1;

	// res_add_mesh_raw(res_pack, MESH_QUAD, quad_vertices, sizeof(quad_vertices) / sizeof(vertex_t), quad_indices, sizeof(quad_indices) / sizeof(u32));
	res_add_mesh(res_pack, MESH_CUBE, load_mesh("res/meshes/cube.mesh"), cube_collider);

	// for (i32 i = 0; i < res_pack.meshes[MESH_CUBE].vertex_count; i++) {
	// 	vertex_t vertex = res_pack.meshes[MESH_CUBE].vertices[i];
	// 	printf("position: %f %f %f\n", vertex.position[0], vertex.position[1], vertex.position[2]);
	// 	printf("uv: %f %f\n", vertex.tex_coord[0], vertex.tex_coord[1]);
	// 	printf("normal: %f %f %f\n", vertex.normal[0], vertex.normal[1], vertex.normal[2]);
	// }

	res_add_mesh(res_pack, MESH_FLOOR, load_mesh("res/meshes/floor.mesh"), floor_collider);
	res_add_mesh(res_pack, MESH_SLAB, load_mesh("res/meshes/slab.mesh"), slab_collider);
	res_add_mesh(res_pack, MESH_SLOPE, load_mesh("res/meshes/slope.mesh"), (collider_t){0});
	res_add_mesh(res_pack, MESH_PYRAMID, load_mesh("res/meshes/pyramid.mesh"), cube_collider);
	res_add_mesh(res_pack, MESH_CORNER, load_mesh("res/meshes/corner.mesh"), (collider_t){0});
	res_add_mesh(res_pack, MESH_MONKEY, load_mesh("res/meshes/monkey.mesh"), (collider_t){0});
	res_add_mesh(res_pack, MESH_MUSHROOM, load_mesh("res/meshes/mushroom.mesh"), (collider_t){0});
	res_add_mesh(res_pack, MESH_WALL, load_mesh("res/meshes/wall.mesh"), wall_collider);
	res_add_mesh(res_pack, MESH_WALL_CORNER, load_mesh("res/meshes/wall_corner.mesh"), wall_corner_collider);

	res_add_sound(res_pack, SOUND_JUMP, (sound_t){Mix_LoadWAV("res/sounds/jump.wav")});
}