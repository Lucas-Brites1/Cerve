#include <stdio.h>
#include <stdlib.h>
#include "json.h"

static void increase_json_object_capacity(json_object_t* obj) {
    if(!obj) return;
    obj->capacity *= 2;
    obj->fields = realloc(obj->fields, obj->capacity * sizeof(json_t*));
}

json_object_t* create_json_object() {
    json_object_t* new_json_object = (json_object_t*) malloc(sizeof(json_object_t));
    
    if(!new_json_object) return NULL;

    new_json_object->size=0;
    new_json_object->capacity = INITIAL_CAPACITY;
    new_json_object->fields = calloc(new_json_object->capacity, sizeof(json_t*));

    return new_json_object;
}

boolean add_to_json(json_object_t *json_object, const char *key, void *value, json_type_t value_type) {
    if(!json_object || !key || (value_type != JSON_NULL && !value)) return f;
    
    if(json_object->size >= json_object->capacity) increase_json_object_capacity(json_object);

    json_t* field = (json_t*)malloc(sizeof(json_t));
    if(!field) return f;

    field->key = key;
    field->type = value_type;
    switch (value_type) {
        case JSON_STRING:
            field->value.string_val = (const char*)value;
            break;
        case JSON_NUMBER:
            field->value.number_val = *(double*)value;
            break;
        case JSON_BOOL:
            field->value.bool_val = *(boolean*)value;
            break;
        default:
            break;
    }
    
    json_object->fields[json_object->size++] = field;

    return t;
}

char* json_stringfy(json_object_t *json_object) {
    if(!json_object) return NULL;
}
