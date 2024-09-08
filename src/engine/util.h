#pragma once

#include "global.h"

extern i32 window_width;
extern i32 window_height;

char *load_file_to_string(const char path[]);

u8 *load_tga(const char path[]);