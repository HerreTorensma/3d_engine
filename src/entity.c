#include "entity.h"

// static entity_t entities[1024] = {0};
// entity_t entities[1024] = {0};
static ent_system_t *_ent_system = NULL;

void ent_system_init(ent_system_t *ent_system) {
    _ent_system = ent_system;
}

index_t ent_new() {
    for (size_t i = 0; i < 1024; i++) {
        if (!_ent_system->entities[i].is_valid) {
            return i;
        }
    }

    debug_log("Entity array is full.\n");
}

void ent_destroy(index_t index) {
    memset(&_ent_system->entities[index], 0, sizeof(entity_t));
}

entity_t *ent_get(index_t index) {
    return &_ent_system->entities[index];
}