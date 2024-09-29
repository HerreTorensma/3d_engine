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
image_t load_tga(const char path[], bool flip) {
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

gl_color_t color_to_gl_color(color_t color) {
	gl_color_t gl_color = {0};

	gl_color.r = (float)color.r / 255.0f;
	gl_color.g = (float)color.g / 255.0f;
	gl_color.b = (float)color.b / 255.0f;
	gl_color.a = (float)color.a / 255.0f;

	return gl_color;
}