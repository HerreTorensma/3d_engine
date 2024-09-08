#include "util.h"

i32 window_width = 640;
i32 window_height = 480;

// Loads a text file to a char array
// The char array is dynamically allocated so the user is responsible for freeing it
char *load_file_to_string(const char path[]) {
	// Open the file
	FILE *file = fopen(path, "rb");
	if (file == NULL) {
		printf("The text file does not exist\n");
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
u8 *load_tga(const char path[]) {
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		printf("The tga image file does not exist\n");
		return NULL;
	}

	// u8 *buffer;
	u8 id;
	fread(&id, 1, 1, file);

	u8 color_map_type;
	fread(&color_map_type, 1, 1, file);

	u8 image_type;
	fread(&image_type, 1, 1, file);

	u8 color_map_specification[5];
	fread(color_map_specification, 1, 5, file);

	fclose(file);
}