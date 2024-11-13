/*
This file contains the entire entity component system implementation.
For each component a struct and enum entry must be made.
For component ids the i32 type is used because this parameter is typically passed as an enum value which is signed 32 bits.
*/
#pragma once

#include "global.h"

typedef uint64_t entity_t;

typedef struct ecs_query_t {
	entity_t *entities;
	size_t len;
} ecs_query_t;

typedef struct ecs_world_t {
	size_t component_offsets[64];

	void *data;
	size_t block_size;

	u64 *entity_masks;
	u64 capacity;

	entity_t current_entity_id;

	ecs_query_t query;
} ecs_world_t;

#define ECS_REGISTER(ecs, component, id) ecs_register_component(ecs, sizeof(component), id);

void ecs_init(ecs_world_t *ecs);

void ecs_register_component(ecs_world_t *ecs, size_t size, i32 id);

entity_t ecs_new(ecs_world_t *ecs);

void *ecs_get(ecs_world_t *ecs, entity_t entity, i32 id);

void *ecs_set(ecs_world_t *ecs, entity_t entity, i32 id);

void ecs_remove(ecs_world_t *ecs, entity_t entity, i32 id);

bool ecs_has(ecs_world_t *ecs, entity_t entity, i32 id, ...);

void ecs_destroy(ecs_world_t *ecs, entity_t entity);

ecs_query_t ecs_query(ecs_world_t *ecs, i32 id, ...);

void ecs_free(ecs_world_t *ecs);