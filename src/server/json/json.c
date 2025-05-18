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

    field->key = strdup(key);  
    if (!field->key) return f; 

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
        case JSON_OBJECT:
            field->value.object_val = (struct json_object_t*)value;
            break;
        case JSON_ARRAY:
            field->value.array_val = (struct json_array_t*)value;
            break;
        default:
            break;
    }
    
    json_object->fields[json_object->size++] = field;

    return t;
}

char* json_stringify(json_object_t *json_object) {
    if (!json_object) return NULL;
    
    concat_helper_t* result = str_concat("", "{");

    for (size_t i = 0; i < json_object->size; i++) {
        json_t* field = json_object->fields[i];
        const char* key = field->key;
        char* value_str = NULL;

        switch (field->type) {
            case JSON_STRING: 
                const char* val = field->value.string_val;
                int len = get_strlen(val) + 3;
                value_str = malloc(len);
                snprintf(value_str, len, "\"%s\"", val);
                break;
            

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

            case JSON_OBJECT:
                char *inner = json_stringify((json_object_t*)field->value.object_val);
                value_str = strdup(inner);
                break; 
            
            case JSON_ARRAY:
                char* array_str = array_stringify((json_array_t*)field->value.array_val);
                value_str = strdup(array_str);
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

json_array_t* create_json_array() {
    json_array_t* json_array = (json_array_t*) malloc(sizeof(json_array_t));
    if(!json_array) return NULL;

    json_array->capacity = INITIAL_CAPACITY;
    json_array->size = 0;
    json_array->elements = (json_array_element_t**) calloc(json_array->capacity, sizeof(json_array_element_t*));
    if(!json_array->elements) {
        free(json_array);
        return NULL;
    }

    return json_array;
}

boolean increase_json_array_capacity(json_array_t* array) {
    if(!array) return f;

    array->capacity *= 2;

    json_array_element_t** new = (json_array_element_t**)realloc(array->elements, array->capacity * sizeof(json_array_element_t*));

    if(!new) {
        return f;
    }

    array->elements = new; 
    return t;
}

static inline json_array_element_t* create_array_element(void* value, json_type_t type) {
    json_array_element_t* elem = (json_array_element_t*) malloc(sizeof(json_array_element_t));
    if(!elem) return NULL;
    elem->type = type;
    switch(elem->type) {
        case JSON_ARRAY:
            elem->value.array_val = (struct json_array_t*)value;
            break;
        case JSON_OBJECT:
            elem->value.object_val = (struct json_object_t*)value;
            break;
        case JSON_NUMBER:
            elem->value.number_val = *(double*)value;
            break;
        case JSON_BOOL:
            elem->value.bool_val = *(boolean*)value;
            break;
        case JSON_STRING:
            elem->value.string_val = (const char*)value;
            break;
        default:
            break;
    }

    return elem;
}

boolean add_to_array(json_array_t* json_array, void* value, json_type_t type) {
    if(!json_array) return f;
    
    if (json_array->size >= json_array->capacity)  {
        if(!increase_json_array_capacity(json_array)) {
            return f;
        }
    }     
    
    json_array_element_t* element = create_array_element(value, type);
    if(!element) return f;
    
    json_array->elements[json_array->size++] = element;
    return t;
}

char* array_stringify(json_array_t* json_array) {
    if (!json_array) return NULL;

    concat_helper_t* result = str_concat("", "[");
    char* value_str = NULL;
    
    for(size_t i=0; i<json_array->size; i++) {
        json_array_element_t* element = json_array->elements[i];
        if(!element) continue;

        switch (element->type) {
            case JSON_STRING: 
                const char* val = element->value.string_val;
                int len = get_strlen(val) + 3;
                value_str = malloc(len);
                snprintf(value_str, len, "\"%s\"", val);
                break;
            
            case JSON_NUMBER:
                value_str = malloc(64);
                snprintf(value_str, 64, "%.15g", element->value.number_val);
                break;

            case JSON_BOOL:
                value_str = strdup(element->value.bool_val ? "true" : "false");
                break;

            case JSON_NULL:
                value_str = strdup("null");
                break;

            case JSON_OBJECT:
                char *inner_object = json_stringify((json_object_t*)element->value.object_val);
                value_str = inner_object;
                break; 

            case JSON_ARRAY:
                char *inner_array = array_stringify((json_array_t*)element->value.array_val);
                value_str = inner_array;
                break;
        }

        concat_helper_t* new_result = str_concat(result->string_concatened, value_str);
        free(result->string_concatened);
        free(result);
        result = new_result;

        if(i < json_array->size - 1) {
            new_result = str_concat(result->string_concatened, ",");
            free(result->string_concatened);
            free(result);
            result = new_result;
        }

        free(value_str);
    }

    concat_helper_t* closed = str_concat(result->string_concatened, "]");
    free(result->string_concatened);
    free(result);

    return closed->string_concatened;
}

