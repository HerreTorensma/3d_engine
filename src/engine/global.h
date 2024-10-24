#pragma once

// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

// Vendor libraries
#include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>
#include <glad/glad.h>
#include <cglm/cglm.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

enum ortho_view {
    ORTHO_TOP,
    ORTHO_BOTTOM,
    ORTHO_FRONT,
    ORTHO_BACK,
    ORTHO_LEFT,
    ORTHO_RIGHT,
};

typedef struct transform {
	vec3 position;
	vec3 rotation;
	vec3 scale;
} transform_t;

typedef transform_t transform_c;

typedef struct {
	size_t texture_index;
	bool billboard;

    float x_scale;
    float y_scale;
} sprite_c;

typedef struct {
    size_t mesh_index;
    size_t texture_index;
} mesh_c;

typedef struct {
    vec3 speed;
} rotating_c;