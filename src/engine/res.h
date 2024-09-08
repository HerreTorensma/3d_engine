#pragma once

#include "global.h"
#include "mesh.h"

typedef struct res_pack {
    mesh_t meshes[256];
} res_pack_t;

void res_add_mesh_raw(res_pack_t *res_pack, size_t index, vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count);