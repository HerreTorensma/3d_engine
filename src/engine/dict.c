#include "dict.h"

// #define FNV_offset_basis 0x811c9dc5
// #define FNV_prime 0x01000193
#define FNV_offset_basis 0xcbf29ce484222325UL
#define FNV_prime 0x100000001b3UL

#define INITIAL_CAPACITY 16

// Using FNV-1a hash
static u64 dict_hash(const char key[]) {
	u64 hash = FNV_offset_basis;

	for (size_t i = 0; i < strlen(key); i++) {
		hash ^= key[i];
		hash *= FNV_prime;
	}

	return hash;
}

//TODO: make sure you cant set the same key twice
static void dict_set_item(struct dict_item_t **items, const char key[], void *value, size_t index) {
	// Navigate linked list
	struct dict_item_t *new_item = malloc(sizeof(struct dict_item_t));
	if (new_item == NULL) {
		return;
	}

	new_item->key = malloc(strlen(key) + 1);
	if (new_item->key == NULL) {
		free(new_item);
		return;
	}
	strcpy(new_item->key, key);

	new_item->value = value;

	// If there is an item in the slot then
	// set the new item next pointer to that
	if (items[index] != NULL) {
		new_item->next = items[index];
	} else {
		new_item->next = NULL;
	}
	
	items[index] = new_item;
}

static struct dict_item_t *dict_get_item(dict_t *dict, const char key[]) {
	u64 hash = dict_hash(key);
	size_t index = (size_t)(hash & (u64)(dict->capacity - 1));

	struct dict_item_t *item = dict->items[index];
	while (item != NULL && strcmp(key, item->key) != 0) {
		item = item->next;
	}

	if (item == NULL) {
		return NULL;
	}

	// We got the right item
	return item;
}

static void dict_resize(dict_t *dict, size_t new_capacity) {
	// Make a new items array
	struct dict_item_t **new_items = calloc(new_capacity, sizeof(struct dict_item_t));
	if (new_items == NULL) {
		return;
	}

	// Go through the old items array
	for (size_t i = 0; i < dict->capacity; i++) {
		struct dict_item_t *item = dict->items[i];

		while (item != NULL) {
			struct dict_item_t *next = item->next;
			u64 hash = dict_hash(item->key);
			size_t index = (size_t)(hash & (u64)(new_capacity - 1));
			
			dict_set_item(new_items, item->key, item->value, index);
			
			free(item->key);
			free(item);

			item = next;
		}
	}

	// Free the old items array
	free(dict->items);

	// Set the new items array
	dict->items = new_items;

	dict->capacity = new_capacity;
}

dict_t *dict_create() {
	dict_t *dict = malloc(sizeof(dict_t));
	if (dict == NULL) {
		return NULL;
	}
	
	dict->capacity = INITIAL_CAPACITY;
	dict->length = 0;

	dict->items = calloc(dict->capacity, sizeof(struct dict_item_t));
	if (dict->items == NULL) {
		free(dict);
		return NULL;
	}

	return dict;
}

void dict_set(dict_t *dict, const char key[], void *value) {
	struct dict_item_t *existing_item = dict_get_item(dict, key);
	if (existing_item != NULL) {
		existing_item->value = value;
		return;
	}

	if (dict->length >= dict->capacity) {
		dict_resize(dict, dict->capacity * 2);
	}

	u64 hash = dict_hash(key);
	size_t index = (size_t)(hash & (u64)(dict->capacity - 1));

	dict_set_item(dict->items, key, value, index);
	dict->length++;

}

void *dict_get(dict_t *dict, const char key[]) {
	struct dict_item_t *item = dict_get_item(dict, key);

	if (item == NULL) {
		return NULL;
	}

	// We got the right item
	return item->value;
}

void dict_remove(dict_t *dict, const char key[]) {
	if (dict->length <= dict->capacity / 2) {
		dict_resize(dict, dict->capacity / 2);
	}

	u64 hash = dict_hash(key);
	size_t index = (size_t)(hash & (u64)(dict->capacity - 1));

	struct dict_item_t *item = dict->items[index];
	struct dict_item_t *prev = NULL;

	// If the first item has the key
	if (item != NULL && strcmp(key, item->key) == 0) {
		dict->items[index] = item->next;
		free(item->key);
		free(item);
		dict->length--;
		return;
	}

	// Get the previous and current item
	while (item != NULL && strcmp(key, item->key) != 0) {
		prev = item;
		item = item->next;
	}

	if (item == NULL) {
		return;
	}

	prev->next = item->next;

	free(item->key);
	free(item);

	dict->length--;
}

void dict_print(dict_t *dict) {
	for (size_t i = 0; i < dict->capacity; i++) {
		printf("[%d]\n", i);

		struct dict_item_t *item = dict->items[i];
		while (item != NULL) {
			printf("%s: %d\n", item->key, item->value);
			item = item->next;
		}
		printf("\n");
	}
}

void dict_free(dict_t *dict) {
	for (size_t i = 0; i < dict->capacity; i++) {
		struct dict_item_t *item = dict->items[i];

		while (item != NULL) {
			// Remove all linked list items
			struct dict_item_t *next = item->next;
			free(item->key);
			free(item);
			item = next;
		}
	}

	// Free the array
	free(dict->items);

	// Free the dict struct itself
	free(dict);
}