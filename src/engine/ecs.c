#include "ecs.h"

#define INITIAL_CAPACITY 1024

void ecs_init(ecs_world_t *ecs) {
	// Start at 256
	ecs->current_entity_id = 256;

	// ecs->component_ids = dict_create();

	// Malloc 1 because we don't know how big an entity is gonna be yet
	// but the compiler won't wine when we realloc
	ecs->data = malloc(1);

	ecs->capacity = INITIAL_CAPACITY;

	ecs->query.entities = malloc(ecs->capacity * sizeof(entity_t));

	ecs->entity_masks = malloc(ecs->capacity * sizeof(u64));
}

void ecs_register_component(ecs_world_t *ecs, size_t size, u64 id) {
	ecs->component_offsets[id] = ecs->block_size;
	ecs->block_size += size;

	ecs->data = realloc(ecs->data, ecs->capacity * ecs->block_size);
}

static void ecs_resize(ecs_world_t *ecs, size_t new_capacity) {
	ecs->capacity = new_capacity;
	ecs->data = realloc(ecs->data, ecs->capacity * ecs->block_size);
	ecs->entity_masks = realloc(ecs->entity_masks, ecs->capacity * sizeof(u64));
	
	ecs->query.entities = realloc(ecs->query.entities, ecs->capacity * sizeof(entity_t));
}

entity_t ecs_new(ecs_world_t *ecs) {
	if (ecs->current_entity_id >= ecs->capacity) {
		ecs_resize(ecs, ecs->capacity * 2);
	}
	ecs->entity_masks[ecs->current_entity_id] = 0;

	void *entity_ptr = (u8 *)ecs->data + (ecs->current_entity_id * ecs->block_size);

	memset(entity_ptr, 0, ecs->block_size);

	return ecs->current_entity_id++;
}

void *ecs_get(ecs_world_t *ecs, entity_t entity, u64 id) {
	size_t offset = ecs->component_offsets[id];
	return (void *)((u8 *)ecs->data + (entity * ecs->block_size + offset));
}

// void *ecs_set(ecs_world_t *ecs, entity_t entity, u64 id, size_t size) {
void *ecs_set(ecs_world_t *ecs, entity_t entity, u64 id) {
	void *component_ptr = ecs_get(ecs, entity, id);
	
	// memset(component_ptr, 0, size);

	ecs->entity_masks[entity] |= (1 << id);

	return component_ptr;
}

void ecs_remove(ecs_world_t *ecs, entity_t entity, u64 id) {
	ecs->entity_masks[entity] &= ~(1 << id);
}

bool ecs_has(ecs_world_t *ecs, entity_t entity, i64 id, ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, id);
	// printf("id: %ld\n", id);

	while (id != -1) {
		// u64 good_idea = id;
		mask |= (1ULL << id);
		// mask |= (1ULL << good_idea);


		id = va_arg(args, i64);
	}

	va_end(args);

	if (mask == (ecs->entity_masks[entity] & mask)) {
		return true;
	}

	return false;
}

// TODO: recycle the entity id using a queue
void ecs_destroy(ecs_world_t *ecs, entity_t entity) {
	ecs->entity_masks[entity] = 0;
}

// static ecs_query_t ecs_query(ecs_world_t *ecs, ecs_query_t *query, u64 mask) {
// 	query->length = 0;

// 	for (size_t e = 0; e < ecs->current_entity_id; e++) {
// 		if (mask == (ecs->entity_masks[e] & mask)) {
// 			query->entities[query->length] = e;
// 			query->length++;
// 		}
// 	}

// 	return *query;
// }

ecs_query_t ecs_query(ecs_world_t *ecs, i64 id, ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, id);
	printf("id: %ld\n", id);

	while (id != -1) {
		// u64 good_id = id;
		mask |= (1ULL << id);
		// mask |= (1ULL << good_id);

		id = va_arg(args, i64);

		printf("id: %ld\n", id);
	}

	va_end(args);

	ecs->query.length = 0;

	for (size_t e = 0; e < ecs->current_entity_id; e++) {
		if (mask == (ecs->entity_masks[e] & mask)) {
			ecs->query.entities[ecs->query.length] = e;
			ecs->query.length++;
		}
	}

	return ecs->query;

	// return ecs_query(ecs, &(ecs->query), mask);
}

void ecs_free(ecs_world_t *ecs)     {
	free(ecs->entity_masks);
	free(ecs->data);
	free(ecs->query.entities);
}