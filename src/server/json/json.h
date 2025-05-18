#pragma once
#include "./server/types.h"

#define INITIAL_CAPACITY 16

#define TYPE(val) _Generic((val),                \
    const char*: (void*)(val),                   \
    char*: (void*)(val),                         \
    double: (void*)&(val),                       \
    float: (void*)&(val),                        \
    int: (void*)&(val),                          \
    long: (void*)&(val),                         \
    default: (void*)&(val)                       \
)

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

boolean add_json_from_int(json_object_t* obj, const char* key, int value);
boolean add_json_from_float(json_object_t* obj, const char* key, float value);