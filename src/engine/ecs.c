#include "ecs.h"

// #include "components.h"

#define INITIAL_CAPACITY 1024

u64 ecs_id(ecs_world_t *ecs, const char name[]) {
	return (u64)dict_get(ecs->component_ids, name);
}

ecs_world_t ecs_create() {
	ecs_world_t ecs = {0};

	// Start at 256
	ecs.current_entity_id = 256;

	ecs.component_ids = dict_create();

	// Malloc 1 because we don't know how big an entity is gonna be yet
	// but the compiler won't wine when we realloc
	ecs.data = malloc(1);

	ecs.capacity = INITIAL_CAPACITY;

	ecs.query1.entities = malloc(ecs.capacity * sizeof(entity_t));
	ecs.query2.entities = malloc(ecs.capacity * sizeof(entity_t));
	ecs.query3.entities = malloc(ecs.capacity * sizeof(entity_t));

	ecs.entity_masks = malloc(ecs.capacity * sizeof(u64));

	return ecs;
}

void ecs_register_component(ecs_world_t *ecs, const char name[], size_t size) {
	dict_set(ecs->component_ids, name, (void*)ecs->current_component_id);

	ecs->component_offsets[ecs->current_component_id] = ecs->block_size;
	ecs->block_size += size;

	ecs->current_component_id++;

	ecs->data = realloc(ecs->data, ecs->capacity * ecs->block_size);
}

static void ecs_resize(ecs_world_t *ecs, size_t new_capacity) {
	ecs->capacity = new_capacity;
	ecs->data = realloc(ecs->data, ecs->capacity * ecs->block_size);
	ecs->entity_masks = realloc(ecs->entity_masks, ecs->capacity * sizeof(u64));
	
	ecs->query1.entities = realloc(ecs->query1.entities, ecs->capacity * sizeof(entity_t));
	ecs->query2.entities = realloc(ecs->query2.entities, ecs->capacity * sizeof(entity_t));
	ecs->query3.entities = realloc(ecs->query3.entities, ecs->capacity * sizeof(entity_t));
}

entity_t ecs_new(ecs_world_t *ecs) {
	if (ecs->current_entity_id >= ecs->capacity) {
		ecs_resize(ecs, ecs->capacity * 2);
	}
	ecs->entity_masks[ecs->current_entity_id] = 0;

	return ecs->current_entity_id++;
}

void *ecs_get(ecs_world_t *ecs, entity_t entity, u64 id) {
	size_t offset = ecs->component_offsets[id];
	return (u8*)ecs->data + (entity * ecs->block_size + offset);
}

void *ecs_set(ecs_world_t *ecs, entity_t entity, u64 id, size_t size, void *data) {
	void *component_ptr = ecs_get(ecs, entity, id);
	
	memcpy(component_ptr, data, size);

	ecs->entity_masks[entity] |= (1 << id);

	return component_ptr;
}

void ecs_remove(ecs_world_t *ecs, entity_t entity, u64 id) {
	ecs->entity_masks[entity] &= ~(1 << id);
}

bool ecs_has(ecs_world_t *ecs, entity_t entity, const char name[], ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, name);

	while (name != NULL) {
		u64 id = ecs_id(ecs, name);
		mask |= (1 << id);

		name = va_arg(args, const char *);
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

static ecs_query_t ecs_query(ecs_world_t *ecs, ecs_query_t *query, u64 mask) {
	query->length = 0;

	for (size_t e = 0; e < ecs->current_entity_id; e++) {
		if (mask == (ecs->entity_masks[e] & mask)) {
			query->entities[query->length] = e;
			query->length++;
		}
	}

	return *query;
}

ecs_query_t ecs_query1(ecs_world_t *ecs, const char name[], ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, name);

	while (name != NULL) {
		u64 id = ecs_id(ecs, name);
		mask |= (1 << id);

		name = va_arg(args, const char *);
	}

	va_end(args);

	return ecs_query(ecs, &(ecs->query1), mask);
}

ecs_query_t ecs_query2(ecs_world_t *ecs, const char name[], ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, name);

	while (name != NULL) {
		u64 id = ecs_id(ecs, name);
		mask |= (1 << id);

		name = va_arg(args, const char *);
	}

	va_end(args);

	return ecs_query(ecs, &(ecs->query2), mask);
}

ecs_query_t ecs_query3(ecs_world_t *ecs, const char name[], ...) {
	u64 mask = 0;
	
	va_list args;
	va_start(args, name);

	while (name != NULL) {
		u64 id = ecs_id(ecs, name);
		mask |= (1 << id);

		name = va_arg(args, const char *);
	}

	va_end(args);

	return ecs_query(ecs, &(ecs->query3), mask);
}

void ecs_free(ecs_world_t *ecs)     {
	free(ecs->entity_masks);
	free(ecs->data);
	dict_free(ecs->component_ids);

	free(ecs->query1.entities);
	free(ecs->query2.entities);
	free(ecs->query3.entities);

	free(ecs);
}