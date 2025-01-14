/*
This header and source file contain functions related to the resource pack system.
*/
#pragma once

#include "global.h"
#include "core.h"
#include "util.h"

void res_init(res_pack_t *res_pack);

void send_mesh_to_gpu(mesh_t *mesh);

void res_add_mesh(res_pack_t *res_pack, index_t index, const char path[], multi_box_collider_t *collider);

void send_texture_to_gpu(texture_t *texture);

void res_add_texture(res_pack_t *res_pack, index_t index, const char path[]);

void res_add_item(res_pack_t *res_pack, index_t index, item_t item);

inline rect_t tex_get_default_src(res_pack_t *res_pack, index_t texture_index) {
	return (rect_t){
		.x = 0,
		.y = 0,
		.w = res_pack->textures[texture_index].width,
		.h = res_pack->textures[texture_index].height,
	};
}

void res_add_sound(res_pack_t *res_pack, index_t index, sound_t sound);

texture_t *tex_get(index_t index);

mesh_t *mesh_get(index_t index);

void res_add_skybox(res_pack_t *res_pack, const char right_path[], const char left_path[], const char top_path[], const char bottom_path[], const char front_path[], const char back_path[]);