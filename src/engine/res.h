/*
This header and source file contain functions related to the resource pack system.
*/
#pragma once

#include "global.h"
#include "core.h"
#include "util.h"

void send_mesh_to_gpu(mesh_t *mesh);

void res_add_mesh(res_pack_t *res_pack, index_t index, mesh_t mesh, collider_t *collider);

void send_texture_to_gpu(texture_t *texture);

void res_add_texture(res_pack_t *res_pack, index_t index, texture_t texture);

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