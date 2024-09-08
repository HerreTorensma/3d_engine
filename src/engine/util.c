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
// Represented as BGRA
image_t load_tga(const char path[]) {
	image_t image = {0};
	
	// Open file
	FILE *file = fopen(path, "r");
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

	// Read pixel data
	fseek(file, 18, SEEK_SET);
	image.pixels = malloc(image.width * image.height * 4);
	fread(image.pixels, 1, image.width * image.height * 4, file);

	fclose(file);

	return image;
}