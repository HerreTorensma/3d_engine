#include "res.h"

// void res_add_mesh(res_pack_t *res_pack, size_t index, mesh_t *mesh) {
// 	// This line causes a segfault on MacOS because it has no OpenGL 3.3 support
//     // Could be either because this is currently the first OpenGL call in the game,
//     // or because MacOS supports up to OpenGL 2.1 which doesn't support vao's.
// 	glGenVertexArrays(1, &mesh->vao);
// 	glBindVertexArray(mesh->vao);

// 	// Upload vertices
// 	glGenBuffers(1, &mesh->vbo);
// 	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
// 	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(vertex_t), &mesh->vertices[0], GL_STATIC_DRAW);

// 	// Upload indices
// 	glGenBuffers(1, &mesh->ebo);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
// 	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(GLuint), &mesh.indices[0], GL_STATIC_DRAW);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(u32), &mesh->indices[0], GL_STATIC_DRAW);
	
// 	glEnableVertexAttribArray(0);
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));

// 	glEnableVertexAttribArray(1);
// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex_coord));

// 	glBindVertexArray(0);

// 	res_pack->meshes[index] = *mesh;
// }

void res_add_mesh(res_pack_t *res_pack, size_t index, mesh_t mesh) {
	// This line causes a segfault on MacOS because it has no OpenGL 3.3 support
    // Could be either because this is currently the first OpenGL call in the game,
    // or because MacOS supports up to OpenGL 2.1 which doesn't support vao's.
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

void res_add_mesh_raw(res_pack_t *res_pack, size_t index, vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count) {
	mesh_t mesh = {0};

	mesh.vertex_count = vertex_count;
	mesh.index_count = index_count;

	mesh.vertices = malloc(vertex_count * sizeof(vertex_t));
    if (mesh.vertices == NULL) {
        printf("Failed to allocate memory for vertices\n");
    }
	memcpy(mesh.vertices, vertices, vertex_count * sizeof(vertex_t));

	mesh.indices = malloc(index_count * sizeof(u32));
    if (mesh.indices == NULL) {
        printf("Failed to allocate memory for indices\n");
    }
	memcpy(mesh.indices, indices, index_count * sizeof(u32));

    // // This line causes a segfault on MacOS because it has no OpenGL 3.3 support
    // // Could be either because this is currently the first OpenGL call in the game,
    // // or because MacOS supports up to OpenGL 2.1 which doesn't support vao's.
	// glGenVertexArrays(1, &mesh.vao);
	// glBindVertexArray(mesh.vao);

	// // Upload vertices
	// glGenBuffers(1, &mesh.vbo);
	// glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	// glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(vertex_t), &mesh.vertices[0], GL_STATIC_DRAW);

	// // Upload indices
	// glGenBuffers(1, &mesh.ebo);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
	// // glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(GLuint), &mesh.indices[0], GL_STATIC_DRAW);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(u32), &mesh.indices[0], GL_STATIC_DRAW);
	
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));

	// glEnableVertexAttribArray(1);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex_coord));

	// glBindVertexArray(0);

	// res_add_mesh(res_pack, index, &mesh);
	res_add_mesh(res_pack, index, mesh);

	// res_pack->meshes[index] = mesh;
}

void res_add_texture(res_pack_t *res_pack, size_t index, const char path[]) {
	// Generate a new texture in OpenGL
	u32 id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// Set some stuff
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load the actual image data
	image_t image = load_tga(path, true);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.pixels);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.pixels);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_ARGB, GL_UNSIGNED_BYTE, image.pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	free(image.pixels);

	res_pack->texture_ids[index] = id;
}