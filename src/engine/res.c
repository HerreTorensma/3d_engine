#include "res.h"

res_pack_t *_res_pack = NULL;

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

void res_init(res_pack_t *res_pack) {
	_res_pack = res_pack;
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

void res_add_mesh(res_pack_t *res_pack, index_t index, const char path[], multi_box_collider_t *collider) {
	if (!index_valid(index)) {
		return;
	}

	mesh_t mesh = load_mesh(path);

	send_mesh_to_gpu(&mesh);

	// mesh.collider = collider;
	if (collider != NULL) {
		memcpy(&mesh.collider, collider, sizeof(multi_box_collider_t));
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

static u32 send_cube_map_to_gpu(texture_t *right_texture, texture_t *left_texture, texture_t *top_texture, texture_t *bottom_texture, texture_t *front_texture, texture_t *back_texture) {
	u32 id = 0;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	{
		// texture_t right = load_tga("res/images/skybox/right.tga");
		// texture_t left = load_tga("res/images/skybox/left.tga");
		// texture_t top = load_tga("res/images/skybox/top.tga");
		// texture_t bottom = load_tga("res/images/skybox/bottom.tga");
		// texture_t front = load_tga("res/images/skybox/front.tga");
		// texture_t back = load_tga("res/images/skybox/back.tga");

		texture_t right = load_tga("res/images/skybox/top.tga");
		texture_t left = load_tga("res/images/skybox/top.tga");
		texture_t top = load_tga("res/images/skybox/top.tga");
		texture_t bottom = load_tga("res/images/skybox/top.tga");
		texture_t front = load_tga("res/images/skybox/top.tga");
		texture_t back = load_tga("res/images/skybox/top.tga");
		
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, right.width, right.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, right.pixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, left.width, left.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, left.pixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, top.width, top.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, top.pixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, bottom.width, bottom.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bottom.pixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, front.width, front.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, front.pixels);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, back.width, back.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, back.pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return id;
}

void res_add_texture(res_pack_t *res_pack, index_t index, const char path[]) {
	if (!index_valid(index)) {
		return;
	}

	texture_t texture = load_tga(path);

	send_texture_to_gpu(&texture);

	res_pack->textures[index] = texture;
}

void res_add_sound(res_pack_t *res_pack, index_t index, sound_t sound) {
	res_pack->sounds[index] = sound;
}

void res_add_item(res_pack_t *res_pack, index_t index, item_t item) {
	if (!index_valid(index)) {
		return;
	}

	res_pack->items[index] = item;
}

texture_t *tex_get(index_t index) {
	if (!index_valid(index)) {
		return NULL;
	}

	return &_res_pack->textures[index];
}

mesh_t *mesh_get(index_t index) {
	if (!index_valid(index)) {
		return NULL;
	}

	return &_res_pack->meshes[index];
}

void res_add_skybox(res_pack_t *res_pack, const char right_path[], const char left_path[], const char top_path[], const char bottom_path[], const char front_path[], const char back_path[]) {
	texture_t right_texture = load_tga(right_path);
	texture_t left_texture = load_tga(left_path);
	texture_t top_texture = load_tga(top_path);
	texture_t bottom_texture = load_tga(bottom_path);
	texture_t front_texture = load_tga(front_path);
	texture_t back_texture = load_tga(back_path);
	
	res_pack->skybox_id = send_cube_map_to_gpu(&right_texture, &left_texture, &top_texture, &bottom_texture, &front_texture, &back_texture);
}