#pragma once

#include "engine/global.h"
#include "engine/core.h"
#include "gamedefs.h"

typedef struct ent_system {
    entity_t entities[1024];
    // size_t used_entities;
} ent_system_t;

// entity_t entities[1024] = {0};
// extern entity_t entities[1024];

index_t ent_new();

void ent_destroy(index_t index);

entity_t *ent_get(index_t index);