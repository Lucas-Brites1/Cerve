#pragma once
#include <stdio.h>
#include "json.h"
#include "./server/types.h"

boolean json_add_int(json_object_t* obj, const char* key, int value) {
    if (value == 0 || value == 1) {
        boolean b = value ? t : f;
        return add_to_json(obj, key, &b, JSON_BOOL);  
    }

    double v = (double)value;
    return add_to_json(obj, key, &v, JSON_NUMBER); 
}

static inline boolean json_add_float(json_object_t* obj, const char* key, float val) {
    return add_json_from_float(obj, key, val);
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

#define JSON(obj, key, value) \
    _Generic((value), \
        int: json_add_int, \
        float: json_add_float, \
        double: json_add_double, \
        char*: json_add_str, \
        const char*: json_add_str \
    )(obj, key, value)
