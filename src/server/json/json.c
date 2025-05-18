#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "json.h"
#include "./server/utils/utils.h"

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
    if (!json_object) return NULL;
    
    concat_helper_t* result = str_concat("", "{");

    for (size_t i = 0; i < json_object->size; i++) {
        json_t* field = json_object->fields[i];
        const char* key = field->key;
        char* value_str = NULL;

        switch (field->type) {
            case JSON_STRING: {
                const char* val = field->value.string_val;
                int len = get_strlen(val) + 3;
                value_str = malloc(len);
                snprintf(value_str, len, "\"%s\"", val);
                break;
            }

            case JSON_NUMBER:
                value_str = malloc(64);
                snprintf(value_str, 64, "%.15g", field->value.number_val);
                break;

            case JSON_BOOL:
                value_str = strdup(field->value.bool_val ? "true" : "false");
                break;

            case JSON_NULL:
                value_str = strdup("null");
                break;
        }

        int pair_len = get_strlen(key) + get_strlen(value_str) + 6;
        char* pair = malloc(pair_len);
        snprintf(pair, pair_len, "\"%s\":%s%s", key, value_str, (i < json_object->size - 1) ? "," : "");

        concat_helper_t* new_result = str_concat(result->string_concatened, pair);
        free(result->string_concatened);
        free(result);

        result = new_result;

        free(value_str);
        free(pair);
    }

    concat_helper_t* closed = str_concat(result->string_concatened, "}");
    free(result->string_concatened);
    free(result);

    return closed->string_concatened;
}

boolean add_json_from_int(json_object_t* obj, const char* key, int value) {
    double v = (double)value;
    return add_to_json(obj, key, &v, JSON_NUMBER);
}

boolean add_json_from_float(json_object_t* obj, const char* key, float value) {
    double v = (double)value;
    return add_to_json(obj, key, &v, JSON_NUMBER);
}
