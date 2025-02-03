#include "engine.h"

static bool running = true;

static SDL_Window *window = NULL;
static SDL_GLContext *context = NULL;

static bool fullscreen = false;

static i32 render_width = 0;
static i32 render_height = 0;

static void (*_input_function)(SDL_Event event);

static SDL_Window *create_sdl2_window(const char title[], i32 width, i32 height) {

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Initialize SDL2
	if (SDL_Init(SDL_INIT_EVENTS) < 0) {
		debug_log("Failed to initialize SDL2: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Initialize audio
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("Failed to open audio\n");
		return EXIT_FAILURE;
	}

	// Create the window
	SDL_Window *window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!window) {
		debug_log("Failed to create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	return window;
}

static SDL_GLContext *create_sdl2_gl_context(SDL_Window *window, i32 width, i32 height) {
	// Make an OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
		debug_log("Failed to create OpenGL context: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		debug_log("Failed to load OpenGL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	debug_log("OpenGL loaded\n");
	debug_log("Vendor: %s\n", glGetString(GL_VENDOR));
	debug_log("Renderer: %s\n", glGetString(GL_RENDERER));
	debug_log("Version: %s\n", glGetString(GL_VERSION));
	
	glViewport(0, 0, width, height);

	return context;
}

static void resize_window(SDL_Window *window, i32 render_width, i32 render_height) {
	i32 w, h;
	SDL_GetWindowSize(window, &w, &h);

	// glViewport(0, 0, w, h);
	
	window_width = w;
	window_height = h;

	screen_scale = min_i32(window_width / render_width, window_height / render_height);
    if (screen_scale <= 0) {
        screen_scale = 1;
    }

	x_offset = (window_width / 2) - ((render_width * screen_scale) / 2);
	y_offset = (window_height / 2) - ((render_height * screen_scale) / 2);
	

	viewport_width = render_width * screen_scale;
	viewport_height = render_height * screen_scale;

	glViewport(x_offset, y_offset, viewport_width, viewport_height);
}

void engine_init(const char window_title[], i32 width, i32 height, i32 _render_width, i32 _render_height, void (*input_function)(SDL_Event event)) {
    window_width = width;
    window_height = height;

    render_width = _render_width;
    render_height = _render_height;

    window = create_sdl2_window(window_title, window_width, window_height);
	context = create_sdl2_gl_context(window, window_width, window_height);
    resize_window(window, render_width, render_height);

    _input_function = input_function;

    // 16 megabytes
    temp_init(16777216);
}

void engine_update() {
    input_update();
    temp_clear();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            resize_window(window, render_width, render_height);
        }

        _input_function(event);
    }

    if (input_key_pressed(SDL_SCANCODE_F11)) {
        fullscreen = !fullscreen;
        if (fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        } else {
            SDL_SetWindowFullscreen(window, 0);
        }
    }
}

void engine_update_end() {
    SDL_GL_SwapWindow(window);
}

void engine_quit() {
    debug_log("Quitting\n");

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool window_should_close() {
    return !running;
}
