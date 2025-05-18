#pragma once
#include <stdio.h>
#include "json.h"
#include "./server/types.h"

static inline boolean json_add_int(json_object_t* obj, const char* key, int value) {
    double v = (double)value;  
    return add_to_json(obj, key, &v, JSON_NUMBER);  
}

#define JSON_BOOLEAN(obj, key, val) json_add_bool(obj, key, val)
static inline boolean json_add_bool(json_object_t* obj, const char* key, boolean val) {
    return add_to_json(obj, key, &val, JSON_BOOL);
}

static inline boolean json_add_float(json_object_t* obj, const char* key, float val) {
    double v = (double)val;
    return add_to_json(obj, key, &v, JSON_NUMBER);
}


static inline boolean json_add_double(json_object_t* obj, const char* key, double val) {
    return add_to_json(obj, key, &val, JSON_NUMBER);
}

static inline boolean json_add_str(json_object_t* obj, const char* key, const char* val) {
    return add_to_json(obj, key, (void*)val, JSON_STRING);
}

static inline boolean json_add_null(json_object_t* obj, const char* key) {
    return add_to_json(obj, key, NULL, JSON_NULL);
} 

static inline boolean json_add_obj(json_object_t* obj, const char* key, json_object_t* obj2) {
    return add_to_json(obj, key, obj2, JSON_OBJECT);
}

static inline boolean json_add_arr(json_object_t* obj, const char* key, json_array_t* arr) {
    return add_to_json(obj, key, arr, JSON_ARRAY);
}

static inline boolean arr_add_int(json_array_t* arr, int value) {
     if (value == 0 || value == 1) {
        boolean b = value ? t : f;
        return add_to_array(arr, &b, JSON_BOOL);  
    }

    double v = (double)value;
    return add_to_array(arr, &v, JSON_NUMBER); 
}

#define JSON(obj, key, value)                       \
    _Generic((value),                               \
        int: json_add_int,                          \
        float: json_add_float,                      \
        double: json_add_double,                    \
        char*: json_add_str,                        \
        const char*: json_add_str,                  \
        json_object_t* : json_add_obj,              \
        json_array_t* : json_add_arr                \
    )(obj, key, value)


#define JSON_NULLISH(obj, key)                      \
    json_add_null(obj, key)                         \

//json_array_t* : json_add_array

static inline boolean add_int_to_array(json_array_t* arr, int value) {
    double v = (double)value;
    return add_to_array(arr, &v, JSON_NUMBER);
}

static inline boolean add_json_float_to_array(json_array_t* arr, float value) {
    double v = (double)value;
    return add_to_array(arr, &v, JSON_NUMBER);
}

static inline boolean add_json_double_to_array(json_array_t* arr, double value) {
    return add_to_array(arr, &value, JSON_NUMBER);
}

static inline boolean add_json_str_to_array(json_array_t* arr, const char* value) {
    return add_to_array(arr, (void*)value, JSON_STRING);
}

static inline boolean add_json_null_to_array(json_array_t* arr) {
    return add_to_array(arr, NULL, JSON_NULL);
}

#define APPEND_NULL(arr) \
    add_json_null_to_array(arr) \

#define APPEND_BOOL(arr, bool) add_json_bool_to_array(arr, bool);
static inline boolean add_json_bool_to_array(json_array_t* arr, boolean value) {
    return add_to_array(arr, &value, JSON_BOOL);
}

static inline boolean add_json_object_to_array(json_array_t* arr, json_object_t* obj) {
    return add_to_array(arr, obj, JSON_OBJECT);
}

static inline boolean add_json_array_to_array(json_array_t* arr, json_array_t* arr2) {
    return add_to_array(arr, arr2, JSON_ARRAY);
}

#define APPEND(arr, value) \
    _Generic((value), \
        int: add_int_to_array, \
        float: add_json_float_to_array, \
        double: add_json_double_to_array, \
        char*: add_json_str_to_array, \
        const char*: add_json_str_to_array, \
        json_object_t*: add_json_object_to_array, \
        json_array_t*: add_json_array_to_array \
    )(arr, value)
