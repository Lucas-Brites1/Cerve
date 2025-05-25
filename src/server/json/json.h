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
char* serialize(json_object_t *json_object);

json_array_t* create_json_array();
boolean add_to_array(json_array_t* json_array, void* value, json_type_t type);
char* array_serialize(json_array_t* json_array);

typedef enum {
	FIELD_STRING,
	FIELD_NUMBER,
	FIELD_BOOL,
	FIELD_OBJECT,
	FIELD_ARRAY,
	FIELD_NULL
} field_type_t;


typedef enum {
    ARRAY_INT,
    ARRAY_FLOAT,
    ARRAY_DOUBLE,
    ARRAY_STRING,
    ARRAY_BOOL,
    ARRAY_OBJECT,
    ARRAY_GENERIC
} array_type_t;

typedef struct field_descriptor{
	field_type_t field_type;
	const char* field_key;
	size_t field_offset;
	struct field_descriptor*  sub_descriptor;
	size_t object_size;
	array_type_t array_type;
} field_descriptor_t;

typedef struct {
	void* element_value;
	field_type_t element_type;
} field_array_element_t;

typedef struct {
    field_array_element_t* elements; 
	array_type_t array_type;
    int count;                  
    int capacity;               
} field_array_t;

void deserialize(const char* serialized_json, void *struct_to_deserialize_ptr, field_descriptor_t* descriptors);
void deserialize_array(
    array_type_t type,
    const char* serialized_json,
    const char* array_key,
    void* struct_to_deserialize_ptr,
    size_t array_field_offset,
    field_descriptor_t* object_descriptor // usado apenas se for ARRAY_OBJECT
);

field_array_t* create_array_to_deserialize(array_type_t array_type);
void add_to_deserializer_array(field_array_t* arr, void* value, field_type_t value_type);