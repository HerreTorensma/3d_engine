#pragma once

#include "engine/global.h"
#include "engine/core.h"
#include "gamedefs.h"

#define MAX_ENTITIES 1024

typedef struct ent_system {
    entity_t entities[MAX_ENTITIES];
    size_t capacity;
} ent_system_t;

void ent_system_init(ent_system_t *ent_system);

index_t ent_new();

void ent_destroy(index_t index);

entity_t *ent_get(index_t index);