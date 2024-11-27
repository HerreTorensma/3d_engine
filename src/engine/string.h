#pragma once

#include "global.h"

typedef struct string {
    u64 len;
    u8 *data;
} string_t;

#define STR(s) ((string_t){.len = strlen((const u8 *)s), .data = (u8 *)s})