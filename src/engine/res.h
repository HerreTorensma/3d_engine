/*
This header and source file contain functions related to the resource pack system.
*/
#pragma once

#include "global.h"
#include "core.h"
#include "util.h"

void res_add_mesh(res_pack_t *res_pack, index_t index, mesh_t mesh, collision_config_t collision);

void res_add_mesh_raw(res_pack_t *res_pack, index_t index, vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count);

void res_add_texture(res_pack_t *res_pack, index_t index, texture_t texture);

inline rect_t tex_get_default_src(res_pack_t *res_pack, index_t texture_index) {
	return (rect_t){
		.x = 0,
		.y = 0,
		.w = res_pack->textures[texture_index].width,
		.h = res_pack->textures[texture_index].height,
	};
}