#include "util.h"

i32 window_width = 0;
i32 window_height = 0;
i32 screen_scale = 0;
i32 x_offset = 0;
i32 y_offset = 0;
i32 viewport_width = 0;
i32 viewport_height = 0;

// Loads a text file to a char array
// The char array is dynamically allocated so the user is responsible for freeing it
char *load_file_to_string(const char path[]) {
	// Open the file
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		printf("%s does not exist\n", path);
		return NULL;
	}

	// Get the file size
	fseek(file, 0, SEEK_END);
	u32 file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	char *buffer = malloc(file_size + 1);
	if (buffer == NULL) {
		printf("Can't allocate memory\n");
		fclose(file);
		return NULL;
	}

	fread(buffer, 1, file_size, file);

	buffer[file_size] = '\0';

	fclose(file);

	return buffer;
}

// Loads a tga image file to a byte array that can be used by OpenGL as a texture
// Represented as BGRA
texture_t load_tga(const char path[], bool flip) {
	texture_t image = {0};
	
	// Open file
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		printf("The tga image file does not exist\n");
		return image;
	}

	// Read and check image type
	fseek(file, 2, SEEK_SET);
	u8 image_type;
	fread(&image_type, 1, 1, file);
	if (image_type != 2) {
		printf("This image type is not supported\n");
		return image;
	}

	// Read width and height
	fseek(file, 12, SEEK_SET);
	fread(&image.width, 2, 1, file);
	fread(&image.height, 2, 1, file);

	u8 *buffer = malloc(image.width * image.height * 4);

	// Read pixel data
	fseek(file, 18, SEEK_SET);
	fread(buffer, 1, image.width * image.height * 4, file);

	fclose(file);

	image.pixels = malloc(image.width * image.height * 4);
	if (flip) {
		// Cool type casting magic
		u32 *pixels_colors = (u32 *)image.pixels;
		u32 *buffer_colors = (u32 *)buffer;

		for (size_t i = 0; i < image.width * image.height; i++) {
			pixels_colors[i] = buffer_colors[image.width * image.height - i - 1];
		}
	} else {
		memcpy(image.pixels, buffer, image.width * image.height * 4);
	}

	free(buffer);

	return image;
}

void color_to_gl_color(color_t color, vec4 gl_color) {
	gl_color[0] = (float)color.r / 255.0f;
	gl_color[1] = (float)color.g / 255.0f;
	gl_color[2] = (float)color.b / 255.0f;
	gl_color[3] = (float)color.a / 255.0f;
}

mesh_t load_mesh_raw(vertex_t *vertices, u32 vertex_count, u32 *indices, u32 index_count) {
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

	send_mesh_to_gpu(&mesh);

	return mesh;
}

mesh_t load_mesh(const char path[]) {
	debug_log("Loading model %s...\n", path);

	mesh_t mesh = {0};

	FILE *file = fopen(path, "rb");

	// Vertex count
	fread(&mesh.vertex_count, sizeof(u32), 1, file);
	debug_log("  Vertex count: %u\n", mesh.vertex_count);
	
	// Allocate memory for vertices and read them
	mesh.vertices = calloc(mesh.vertex_count, sizeof(vertex_t));

	// fread(mesh.vertices, sizeof(vertex_t), mesh.vertex_count, file);
	for (size_t i = 0; i < mesh.vertex_count; i++) {
		fread(&mesh.vertices[i].position[0], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].position[1], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].position[2], sizeof(f32), 1, file);

		fread(&mesh.vertices[i].tex_coord[0], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].tex_coord[1], sizeof(f32), 1, file);

		fread(&mesh.vertices[i].normal[0], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].normal[1], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].normal[2], sizeof(f32), 1, file);
	}

	// Triangle count and indices
	fread(&mesh.index_count, sizeof(u32), 1, file);
	
	// mesh.index_count *= 3;

	debug_log("  Index count: %u\n", mesh.index_count);

	mesh.indices = calloc(mesh.index_count, sizeof(u32));
	
	fread(mesh.indices, sizeof(u32), mesh.index_count, file);

	fclose(file);

	return mesh;
}

// Wrapper so I can change SDL2 dependency
void get_mouse_pos(i32 *x, i32 *y) {
	SDL_GetMouseState(x, y);

	*x -= x_offset;
	*y -= y_offset;

	*x /= screen_scale;
	*y /= screen_scale;
}

// Returns the mouse position normalized to -1.0 to 1.0
void get_normalized_mouse_pos(float *x, float *y) {
	i32 window_x, window_y;
	SDL_GetMouseState(&window_x, &window_y);

	window_x -= x_offset;
	window_y -= y_offset;

	*x = (2.0f * window_x) / viewport_width - 1.0f;
	*y = 1.0f - (2.0f * window_y) / viewport_height;
}

void debug_log(const char *format, ...) {
	#ifdef DEBUG
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	#endif
}

box_t compute_bounding_box(mesh_t *mesh) {
	box_t box = {0};

	box.min_x = mesh->vertices[0].position[0];
	box.max_x = mesh->vertices[0].position[0];

	box.min_y = mesh->vertices[1].position[1];
	box.max_y = mesh->vertices[1].position[1];
	
	box.min_z = mesh->vertices[2].position[2];
	box.max_z = mesh->vertices[2].position[2];
	
	for (u32 i = 0; i < mesh->vertex_count; i++) {
		if (mesh->vertices[i].position[0] < box.min_x) {
			box.min_x = mesh->vertices[i].position[0];
		}
		if (mesh->vertices[i].position[0] > box.max_x) {
			box.max_x = mesh->vertices[i].position[0];
		}

		if (mesh->vertices[i].position[1] < box.min_y) {
			box.min_y = mesh->vertices[i].position[1];
		}
		if (mesh->vertices[i].position[1] > box.max_y) {
			box.max_y = mesh->vertices[i].position[1];
		}

		if (mesh->vertices[i].position[2] < box.min_z) {
			box.min_z = mesh->vertices[i].position[2];
		}
		if (mesh->vertices[i].position[2] > box.max_z) {
			box.max_z = mesh->vertices[i].position[2];
		}
	}

	box.min_x *= 0.5;
	box.max_x *= 0.5;

	box.min_y *= 0.5;
	box.max_y *= 0.5;

	box.min_z *= 0.5;
	box.max_z *= 0.5;

	return box;
}

void print_box(box_t *box) {
	printf("min_x: %f\n", box->min_x);
	printf("max_x: %f\n", box->max_x);
	printf("\n");

	printf("min_y: %f\n", box->min_y);
	printf("max_y: %f\n", box->max_y);
	printf("\n");

	printf("min_z: %f\n", box->min_z);
	printf("max_z: %f\n", box->max_z);
	printf("\n");
}

void print_mesh(res_pack_t *res_pack, index_t mesh_index) {
	for (i32 i = 0; i < res_pack->meshes[mesh_index].vertex_count; i++) {
		vertex_t vertex = res_pack->meshes[mesh_index].vertices[i];
		printf("position: %f %f %f\n", vertex.position[0], vertex.position[1], vertex.position[2]);
		printf("uv: %f %f\n", vertex.tex_coord[0], vertex.tex_coord[1]);
		printf("normal: %f %f %f\n", vertex.normal[0], vertex.normal[1], vertex.normal[2]);
		printf("\n");
	}
}

void resize_window(res_pack_t *res_pack, SDL_Window *window) {
	i32 w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);
	window_width = w;
	window_height = h;
	screen_scale = min_i32(window_width / res_pack->render_width, window_height / res_pack->render_height);
	x_offset = (window_width / 2) - ((res_pack->render_width * screen_scale) / 2);
	y_offset = (window_height / 2) - ((res_pack->render_height * screen_scale) / 2);
	
	viewport_width = res_pack->render_width * screen_scale;
	viewport_height = res_pack->render_height * screen_scale;
}