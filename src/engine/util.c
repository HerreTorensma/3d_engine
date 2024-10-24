#include "util.h"

i32 window_width = 640;
i32 window_height = 480;

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

mesh_t load_mesh(const char path[]) {
	printf("Loading model %s...\n", path);

	mesh_t mesh = {0};

	FILE *file = fopen(path, "rb");

	// Vertex count
	fread(&mesh.vertex_count, sizeof(u32), 1, file);
	printf("Vertex count: %u\n", mesh.vertex_count);
	
	// Allocate memory for vertices and read them
	mesh.vertices = calloc(mesh.vertex_count, sizeof(vertex_t));

	// fread(mesh.vertices, sizeof(vertex_t), mesh.vertex_count, file);
	for (size_t i = 0; i < mesh.vertex_count; i++) {
		fread(&mesh.vertices[i].position[0], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].position[1], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].position[2], sizeof(f32), 1, file);

		fread(&mesh.vertices[i].tex_coord[0], sizeof(f32), 1, file);
		fread(&mesh.vertices[i].tex_coord[1], sizeof(f32), 1, file);
	}

	// Triangle count and indices
	fread(&mesh.index_count, sizeof(u32), 1, file);
	
	// mesh.index_count *= 3;

	printf("Index count: %u\n", mesh.index_count);

	mesh.indices = calloc(mesh.index_count, sizeof(u32));
	
	fread(mesh.indices, sizeof(u32), mesh.index_count, file);

	fclose(file);

	return mesh;
}

// Returns the mouse position normalized to -1.0 to 1.0
void get_normalized_mouse_pos(float *x, float *y) {
	i32 window_x, window_y;
	SDL_GetMouseState(&window_x, &window_y);

	*x = (2.0f * window_x) / window_width - 1.0f;
	*y = 1.0f - (2.0f * window_y) / window_height;
}