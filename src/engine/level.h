#pragma once

#include "global.h"

typedef struct level {
    u32 width;
    u32 height;
    u32 depth;

    u8 map[4096];
} level_t;