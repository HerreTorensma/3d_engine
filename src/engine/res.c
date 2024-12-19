#include "res.h"

res_pack_t _res_pack = {0};

static bool index_valid(index_t index) {
	if (index == 0) {
		debug_log("Index 0 is reserved empty space. Please start from index 1.\n");
		return false;
	}

	if (index >= 256) {
		debug_log("Index %d is above the limit of 255.\n");
		return false;
	}

	return true;
}

void send_mesh_to_gpu(mesh_t *mesh) {
	// This line causes a segfault on MacOS because it has no OpenGL 3.3 support
    // Could be either because this is currently the first OpenGL call in the game,
    // or because MacOS supports up to OpenGL 2.1 which doesn't support vao's.
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Upload vertices
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(vertex_t), &mesh->vertices[0], GL_STATIC_DRAW);

	// Upload indices
	glGenBuffers(1, &mesh->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(GLuint), &mesh.indices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(u32), &mesh->indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, tex_coord));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));

	glBindVertexArray(0);
}

void res_add_mesh(res_pack_t *res_pack, index_t index, mesh_t mesh, collider_t *collider) {
	if (!index_valid(index)) {
		return;
	}

	send_mesh_to_gpu(&mesh);

	// mesh.collider = collider;
	if (collider != NULL) {
		memcpy(&mesh.collider, collider, sizeof(collider_t));
	} else {
		mesh.collider.boxes[0] = compute_bounding_box(&mesh);
		mesh.collider.boxes_len = 1;
	}

	res_pack->meshes[index] = mesh;
}

void send_texture_to_gpu(texture_t *texture) {
	// Generate a new texture in OpenGL
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	// Set some stuff
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width, texture->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void res_add_texture(res_pack_t *res_pack, index_t index, texture_t texture) {
	if (!index_valid(index)) {
		return;
	}

	send_texture_to_gpu(&texture);

	res_pack->textures[index] = texture;
}

void res_add_sound(res_pack_t *res_pack, index_t index, sound_t sound) {
	res_pack->sounds[index] = sound;
}

texture_t *tex_get(index_t index) {
	if (!index_valid(index)) {
		return NULL;
	}

	return &_res_pack.textures[index];
}

mesh_t *mesh_get(index_t index) {
	if (!index_valid(index)) {
		return NULL;
	}

	return &_res_pack.meshes[index];
}