#define _POSIX_C_SOURCE 200809
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

char* serialize(json_object_t *json_object) {
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
                char *inner = serialize((json_object_t*)field->value.object_val);
                value_str = strdup(inner);
                break; 
            
            case JSON_ARRAY:
                char* array_str = array_serialize((json_array_t*)field->value.array_val);
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

char* array_serialize(json_array_t* json_array) {
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
                char *inner_object = serialize((json_object_t*)element->value.object_val);
                value_str = inner_object;
                break; 

            case JSON_ARRAY:
                char *inner_array = array_serialize((json_array_t*)element->value.array_val);
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


// Deserializer.c
void deserialize(const char* serialized_json, void *struct_to_deserialize_ptr, field_descriptor_t* descriptors) {
    for(int i = 0; descriptors[i].field_key != NULL; i++) {
        field_descriptor_t* current_field = &descriptors[i];
        if (!current_field) continue;

        const char* key = current_field->field_key;
        field_type_t type = current_field->field_type;
        size_t offset = current_field->field_offset;

        char* key_pos = mv_stringtosubstring(serialized_json, (char*)key);
        if (!key_pos) continue;

        char* clean_value = strcopy_start_end(key_pos, ':', ',');
        if (!clean_value) continue;

        void* field_ptr = (char*)struct_to_deserialize_ptr + offset;

        switch (type) {
            case FIELD_NUMBER: {
                int value = atoi(clean_value);
                memcpy(field_ptr, &value, sizeof(int));
                break;
            }
            case FIELD_STRING: {
                char* heap_str = strdup(clean_value);
                if (!heap_str) {
                    free(clean_value);
                    return;
                }
                memcpy(field_ptr, &heap_str, sizeof(char*)); 
                break;
            }
            case FIELD_BOOL: {
                int value = 0;
                if (strncmp(clean_value, "true", 4) == 0) value = 1;
                memcpy(field_ptr, &value, sizeof(int));
                break;
            }
            case FIELD_NULL: {
                if (strncmp(clean_value, "null", 4) == 0) {
                    void* null_value = NULL;
                    memcpy(field_ptr, &null_value, sizeof(void*));
                }
                break;
            }
            case FIELD_OBJECT: {
                if (!current_field->sub_descriptor) break;
                
                char* object_start = mv_stringto(key_pos, '{');
                if (!object_start) break;

                char* object_end = find_closing_brace(object_start);
                if(!object_end) break;

                size_t object_len = object_end - object_start + 1;  // Ajuste aqui para pegar o comprimento correto

                char* subjson = strndup(object_start, object_len);  // Corrigir tamanho da string
                if (!subjson) break;

                printf("\n\t\nObject_start: %s\nObject_end: %s\nObject_len: %ld\nSubjson: %s\n", object_start, object_end, object_len, subjson);

                void** struct_field_ptr = (void**)((char*)struct_to_deserialize_ptr + offset);
                *struct_field_ptr = malloc(current_field->object_size); 

                if (!*struct_field_ptr) {
                    free(subjson);
                    break;
                    }
                deserialize(subjson, *struct_field_ptr, current_field->sub_descriptor);
                free(subjson);
                break;
            }
            case FIELD_ARRAY: {
                if(current_field->array_type == ARRAY_OBJECT) {deserialize_array(current_field->array_type, clean_value, key, struct_to_deserialize_ptr, offset, current_field->sub_descriptor); continue;}
                deserialize_array(current_field->array_type, clean_value, key, struct_to_deserialize_ptr, offset, NULL);
            }
            default:
                break;
        }

        free(clean_value);  
    }
}

field_array_t* create_array_to_deserialize(array_type_t array_type) {
    field_array_t* array = (field_array_t*)malloc(sizeof(field_array_t));
    if(!array) return NULL;

    array->array_type = array_type;
    array->capacity = INITIAL_CAPACITY;
    array->count = 0;
    
    array->elements= calloc(INITIAL_CAPACITY, sizeof(field_array_element_t));    
    if(!array->elements) return NULL;
    return array;
}

void add_to_deserializer_array(field_array_t* arr, void* value, field_type_t value_type) {
    if(!arr) return;
    if(arr->array_type != ARRAY_GENERIC) {
        if((int)arr->array_type != (int)value_type) return;
    }

    if(arr->count >= arr->capacity) {
        int new_capacity = arr->capacity * 2;
        field_array_element_t* new_elements = (field_array_element_t*)realloc(arr->elements, sizeof(field_array_element_t) * new_capacity);
        if(!new_elements) return;

        arr->elements = new_elements;
        arr->capacity = new_capacity;
    }

    field_array_element_t new_element = {0};
    new_element.element_type = value_type;
    new_element.element_value = value;
    arr->elements[arr->count++] = new_element;
}

void deserialize_array(
    array_type_t type,
    const char* serialized_json,
    const char* array_key,
    void* struct_to_deserialize_ptr,
    size_t array_field_offset,
    field_descriptor_t* object_descriptor // usado apenas se for ARRAY_OBJECT
) {
    if(!serialized_json || !array_key || !struct_to_deserialize_ptr) return;
    if(type == ARRAY_OBJECT && !object_descriptor) return;

    printf("\nSERIALIZED_JSON: %s", serialized_json);
    printf("Searching for array with key: %s\n", array_key);

    char* array_json = mv_stringto(serialized_json, '\"');
    if (array_json) {
        printf("\n\tARRAY found: %s\n", array_json);
    } else {
        printf("\n\tARRAY not found.\n");
    }

    switch(type) {
        
    }
}