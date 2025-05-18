#pragma once
#include "./server/types.h"

#define INITIAL_CAPACITY 16

typedef enum {
	JSON_STRING,
	JSON_NUMBER,
	JSON_BOOL,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_NULL
} json_type_t;

typedef struct {
	const char* key;
	json_type_t type;
	union {
		const char* string_val;
		double number_val;
		boolean bool_val;
		struct json_object_t* object_val;
		struct json_array_t* array_val;
	} value;
} json_t;

typedef struct {
	json_type_t type;
	union {
		const char* string_val;
		double number_val;
		boolean bool_val;
		struct json_object_t* object_val;
		struct json_array_t* array_val;
	} value;
} json_array_element_t;

typedef struct {
	json_array_element_t **elements;	
	size_t capacity;
	size_t size;
} json_array_t;

typedef struct {
	json_t **fields;
	size_t size;
	size_t capacity;
} json_object_t;

json_object_t* create_json_object();
boolean add_to_json(json_object_t *json_object, const char *key, void *value, json_type_t value_type);
char* json_stringify(json_object_t *json_object);

json_array_t* create_json_array();
boolean add_to_array(json_array_t* json_array, void* value, json_type_t type);
char* array_stringify(json_array_t* json_array);