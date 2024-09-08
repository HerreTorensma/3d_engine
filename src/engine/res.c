#include "res.h"

void res_add_mesh_raw(res_pack_t *res_pack, size_t index, vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count) {
	mesh_t mesh = {0};

	mesh.vertex_count = vertex_count;
	mesh.index_count = index_count;

	mesh.vertices = malloc(vertex_count * sizeof(vertex_t));
	memcpy(mesh.vertices, vertices, vertex_count * sizeof(vertex_t));

	mesh.indices = malloc(index_count * sizeof(u32));
	memcpy(mesh.indices, indices, index_count * sizeof(u32));

	glGenVertexArrays(1, &mesh.vao);
	glBindVertexArray(mesh.vao);

	// Upload vertices
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(vertex_t), &mesh.vertices[0], GL_STATIC_DRAW);

	// Upload indices
	glGenBuffers(1, &mesh.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(GLuint), &mesh.indices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), &mesh.indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex_coord));

	glBindVertexArray(0);

	res_pack->meshes[index] = mesh;
}