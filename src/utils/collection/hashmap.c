#include "hashmap.h"

#include "hashtable-impl.h"
#include "gnulib/error.h"
#include "utils/memory/allocator.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

extern int errno;

extern allocator_t* main_allocator;

typedef struct _hashmap_item_t {
	void *key;
	void* item;
	hashmap_t* owner;
} hashmap_item_t;

struct _hashmap_t {
	hashtable_t* hashtable;
	coll_hash_f key_hash;
	coll_equals_f key_equals;
};

// Item functions *********************************************************

static bool item_eq(const void* litem, const void* ritem) {
	hashmap_item_t* lhashmap_item = (hashmap_item_t*)litem;
	hashmap_item_t* rhashmap_item = (hashmap_item_t*)ritem;

	return lhashmap_item->owner->key_equals(lhashmap_item->key, rhashmap_item->key);
}

static hashmap_item_t* item_create(hashmap_t* hashmap, void* key, void* item) {
	hashmap_item_t* hashmap_item = malloc(sizeof(hashmap_item_t));
	hashmap_item->key = key;
	hashmap_item->item = item;
	hashmap_item->owner = hashmap;

	return hashmap_item;
}

static uint32_t item_hash(const void *item) {
	const hashmap_item_t* hashmap_item = item;
	hashmap_t* map = hashmap_item->owner;

	return map->key_hash(hashmap_item->key);
}

typedef struct {
	coll_predicate_t* key_predicate;
} evaluate_map_item_data_t;

static bool evaluate_map_item(const void* item, void* data) {
	const hashmap_item_t* hashmap_item = item;
	evaluate_map_item_data_t* ev_data = data;

	return ev_data->key_predicate->evaluate(hashmap_item->key, ev_data->key_predicate->data);
}


// Interface funcions *****************************************************

hashmap_t* hashmap_create(coll_equals_f key_equals, coll_hash_f key_hash, hashmap_options_t* options) {

	hashmap_t* map = malloc(sizeof(hashmap_t));

	map->key_hash = key_hash;
	map->key_equals = key_equals;

	hashtable_options_t hashtable_options;
	if(options!=NULL) {
		hashtable_options.initial_capacity = options->initial_capacity;
		hashtable_options.load_factor = options->load_factor;
	}

	map->hashtable = hashtable_create(&item_eq, &item_hash, &hashtable_options);

	return map;
}


void* hashmap_put(hashmap_t* map, const void* key, const void* item) {

	// item can be NULL
	assert(map!=NULL);
	assert(key!=NULL);

	hashmap_item_t* hashmap_item = item_create(map, key, item);
	hashmap_item_t* replaced = hashtable_add(map->hashtable, hashmap_item);

	return replaced;
}

void* hashmap_get(hashmap_t* map, const void* key) {

	assert(map!=NULL);
	assert(key!=NULL);

	void* item = NULL;

	hashmap_item_t map_item;
	map_item.key = key;
	map_item.owner = map;

	hashmap_item_t* found = hashtable_get(map->hashtable, &map_item);

	if(found){
		item = found->item;
		// FIXME use allocator
		free(found);
	}

	return item;
}

void* hashmap_remove(hashmap_t* map, const void* key) {

	assert(map!=NULL);
	assert(key!=NULL);

	void* item = NULL;

	hashmap_item_t hashMapItem;
	hashMapItem.key = key;
	hashMapItem.owner = map;

	hashmap_item_t* removed = hashtable_remove(map->hashtable, &hashMapItem);

	if(removed){
		item = removed->item;
		// FIXME use allocator
		free(removed);
	}

	return item;
}

uint32_t hashmap_size(hashmap_t* map){

	assert(map!=NULL);

	return hashtable_size(map->hashtable);
}

int hashmap_empty(hashmap_t* hashmap) {

	assert(hashmap!=NULL);

	return hashmap_size(hashmap)==0;
}

int hashmap_contains(hashmap_t* hashmap, const void* key) {

	assert(hashmap!=NULL);

	return hashmap_get(hashmap, key) != NULL;
}

void hashmap_clear(hashmap_t* map) {

	assert(map!=NULL);

	hashtable_clear(map->hashtable, NULL);
}

void hashmap_destroy(hashmap_t* map) {

	assert(map!=NULL);

	hashtable_destroy(map->hashtable, NULL);

	// FIXME use allocator
	free(map);
}

void* hashmap_find(hashmap_t* map, coll_predicate_t* predicate, uint32_t nth) {

	assert(map!=NULL);
	assert(predicate!=NULL);
	assert(nth>=0);

	void* found = NULL;

	evaluate_map_item_data_t ev_map_item_data;
	ev_map_item_data.key_predicate = predicate;

	coll_predicate_t map_predicate;
	map_predicate.evaluate = &evaluate_map_item;
	map_predicate.data = &ev_map_item_data;

	hashmap_item_t* map_item = hashtable_find(map->hashtable, &map_predicate, nth);
	if(map_item != NULL) {
		found = map_item->item;
	}

	return found;
}

static int hashtable_functor(const void* item, void* data) {
	hashmap_item_t* hashmap_item = item;

	coll_functor_t* map_functor = data;
	return map_functor->functor(hashmap_item->item, map_functor->data);
}

int hashmap_foreach(hashmap_t* hashmap, coll_functor_t* functor) {

	assert(hashmap!=NULL);
	assert(functor!=NULL);

	coll_functor_t ht_functor;
	ht_functor.functor = &hashtable_functor;
	ht_functor.data = functor;

	return hashtable_foreach(hashmap->hashtable, &ht_functor);
}
