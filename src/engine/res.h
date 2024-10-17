#pragma once

#include "global.h"
#include "mesh.h"
#include "util.h"

typedef struct tile {
	size_t mesh_index;
	size_t texture_index;
	vec3 rotation;
} tile_t;

typedef struct res_pack {
	color_t fog_color;
	color_t sky_color;
	color_t editor_color;

    mesh_t meshes[256];
    u32 texture_ids[256];

	tile_t tiles[256];
} res_pack_t;

void res_add_mesh(res_pack_t *res_pack, size_t index, mesh_t mesh);

void res_add_mesh_raw(res_pack_t *res_pack, size_t index, vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count);

void res_add_texture(res_pack_t *res_pack, size_t index, const char path[]);