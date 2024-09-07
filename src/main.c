#include "engine/global.h"
#include "engine/render.h"

#define FPS 60

int main(int argc, char *argv[]) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_EVENTS) < 0) {
		printf("Failed to initialize SDL2: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Create the window
	SDL_Window *window = SDL_CreateWindow(
		"game",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!window) {
		printf("Failed to create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Make an OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
		printf("Failed to create OpenGL context: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		printf("Failed to load OpenGL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	printf("OpenGL loaded\n");
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	
	SDL_SetRelativeMouseMode(SDL_TRUE);

	glViewport(0, 0, 640, 480);

	render_init();

	// Window loop
	bool running = true;
	SDL_Event event;
	while (running) {
		// Quit if there is a quit event
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
				i32 w, h;
				SDL_GetWindowSize(window, &w, &h);
				glViewport(0, 0, w, h);
				window_width = w;
				window_height = h;
			}

			render_input(event);
		}

		input_update();

		if (input_key_pressed(SDL_SCANCODE_ESCAPE)) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}

		render();

		SDL_GL_SwapWindow(window);		

		SDL_Delay(1000/FPS);
	}

	printf("Quitting\n");

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}