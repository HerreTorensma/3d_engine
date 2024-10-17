#pragma once

#include "global.h"

struct dict_item_t {
	// Key
	char *key;
	
	// Value
	void *value;

	// Next item
	struct dict_item_t *next;
};

typedef struct {
	// Array of items
	struct dict_item_t **items;

	// Capacity
	size_t capacity;

	// Amount of items in the dict
	size_t length;
} dict_t;

dict_t *dict_create();

void dict_set(dict_t *dict, const char key[], void *value);

void *dict_get(dict_t *dict, const char key[]);

void dict_remove(dict_t *dict, const char key[]);

void dict_free(dict_t *dict);