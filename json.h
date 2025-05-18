#pragma once
#include "server.h"

#define INITIAL_CAPACITY 16

#define TYPE(val) _Generic((val), 	\
	const char*: (void*)(val), 	\
	char*: (void*)(val), 		\
	default: (void*)&(val)    	\
	)

#define ADD_JSON(obj, key, value, type) \
	add_to_json(obj, key, TYPE(value), type)

typedef enum {
	JSON_STRING,
	JSON_NUMBER,
	JSON_BOOL,
	JSON_NULL
} json_type_t;

typedef struct {
	const char* key;
	json_type_t type;
	union {
		const char* string_val;
		double number_val;
		boolean bool_val;
	} value;
} json_t;

typedef struct {
	json_t **fields;
	size_t size;
	size_t capacity;
} json_object_t;

json_object_t* create_json_object();
boolean add_to_json(json_object_t *json_object, const char *key, void *value, json_type_t value_type);
char* json_stringfy(json_object_t *json_object);
