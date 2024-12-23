/*
This header and source file contain helper functions for working with shaders.
*/
#pragma once

#include "global.h"
#include "util.h"

u32 create_shader_program(const char vertex_path[], const char fragment_path[]);

void shader_set_mat4(u32 id, const char name[], mat4 *value);

void shader_set_int(u32 id, const char name[], int value);

void shader_set_vec4(u32 id, const char name[], vec4 *value);

void shader_set_vec3(u32 id, const char name[], vec3 *value);

void shader_set_vec2(u32 id, const char name[], vec2 *value);