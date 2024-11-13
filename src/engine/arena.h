#pragma once

#include "global.h"

typedef struct arena {
    void *data;
    size_t current_position;
    size_t current_size;
} arena_t;

void arena_init(arena_t *arena, size_t initial_size);

void *arena_alloc(arena_t *arena, size_t size);

void *arena_calloc(arena_t *arena, size_t size);

void arena_clear(arena_t *arena);

void arena_free(arena_t *arena);