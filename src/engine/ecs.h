#pragma once

#include "global.h"

typedef uint64_t entity_t;

struct ecs_world_t;

typedef struct ecs_system_t {
	void (*callback)(struct ecs_world_t *ecs, entity_t entity);
	u64 mask;
} ecs_system_t;

typedef struct ecs_query_t {
	entity_t *entities;
	u64 length;
} ecs_query_t;

typedef struct ecs_world_t {
	// dict_t *component_ids;
	// u64 current_component_id;
	size_t component_offsets[64];

	void *data;
	size_t block_size;

	u64 *entity_masks;
	u64 capacity;

	entity_t current_entity_id;

	ecs_query_t query;
} ecs_world_t;

// // Macro functions
// #define ECS_REGISTER_COMPONENT(ecs, component) ecs_register_component(ecs, #component, sizeof(component))

// // Use like ECS_GET(ecs, entity, position_c)
// #define ECS_GET(ecs, entity, component) (component*)ecs_get(ecs, entity, ecs_id(ecs, #component))

// #define ECS_SET(ecs, entity, component, ...) ecs_set(ecs, entity, ecs_id(ecs, #component), sizeof(component), &((component)__VA_ARGS__))

// #define ECS_REMOVE(ecs, entity, component) ecs_remove(ecs, entity, ecs_id(ecs, #component))

// // Normal functions
// u64 ecs_id(ecs_world_t *ecs, const char name[]);

// ecs_world_t ecs_create();

// void ecs_register_component(ecs_world_t *ecs, const char name[], size_t size);

// entity_t ecs_new(ecs_world_t *ecs);

// void *ecs_get(ecs_world_t *ecs, entity_t entity, u64 id);

// void *ecs_set(ecs_world_t *ecs, entity_t entity, u64 id, size_t size, void *data);

// bool ecs_has(ecs_world_t *ecs, entity_t entity, const char name[], ...);

// void ecs_destroy(ecs_world_t *ecs, entity_t entity);

// void split_query(ecs_world_t *ecs, const char components[]);

// ecs_query_t ecs_query(ecs_world_t *ecs, const char name[], ...);

// void ecs_free(ecs_world_t *ecs);

#define ECS_REGISTER(ecs, component, id) ecs_register_component(ecs, sizeof(component), id);

// #define ECS_SET(ecs, entity, id, component) ecs_set(ecs, entity, sizeof(component), id);

void ecs_init(ecs_world_t *ecs);

void ecs_register_component(ecs_world_t *ecs, size_t size, u64 id);

entity_t ecs_new(ecs_world_t *ecs);

void *ecs_get(ecs_world_t *ecs, entity_t entity, u64 id);

// void *ecs_set(ecs_world_t *ecs, entity_t entity, size_t size, u64 id);
void *ecs_set(ecs_world_t *ecs, entity_t entity, u64 id);

void ecs_remove(ecs_world_t *ecs, entity_t entity, u64 id);

bool ecs_has(ecs_world_t *ecs, entity_t entity, i64 id, ...);

void ecs_destroy(ecs_world_t *ecs, entity_t entity);

ecs_query_t ecs_query(ecs_world_t *ecs, i64 id, ...);

void ecs_free(ecs_world_t *ecs);