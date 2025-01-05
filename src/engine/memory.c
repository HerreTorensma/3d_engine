#include "memory.h"

static arena_t _temp_arena;

void arena_init(arena_t *arena, size_t initial_size) {
    arena->current_position = 0;
    arena->current_size = initial_size;
    
    arena->data = malloc(initial_size);
    if (arena->data == NULL) {
        printf("Failed to allocate memory for arena.\n");
    }
}

void *arena_alloc(arena_t *arena, size_t size) {
    if (arena->current_position >= arena->current_size) {
        printf("The arena is full.\n");
    }

    void *ptr = (u8 *)arena->data + arena->current_position;

    arena->current_position += size;

    return ptr;
}

void *arena_calloc(arena_t *arena, size_t size) {
    void *ptr = arena_alloc(arena, size);
    memset(ptr, 0, size);

    return ptr;
}

void arena_clear(arena_t *arena) {
    arena->current_position = 0;
}

void arena_free(arena_t *arena) {
    free(arena->data);
}

void temp_init(size_t initial_size) {
    arena_init(&_temp_arena, initial_size);
}

void *temp_alloc(size_t size) {
    return arena_alloc(&_temp_arena, size);
}

void *temp_calloc(size_t size) {
    return arena_calloc(&_temp_arena, size);
}

void temp_clear() {
    arena_clear(&_temp_arena);
}

void temp_free() {
    arena_free(&_temp_arena);
}