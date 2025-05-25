#pragma once
#include <stdio.h>
#include "./server/server.h"

typedef struct  {
    char *string_concatened;
    size_t len;
} concat_helper_t;

void parser(char *buffer, char *method, char *endpoint, char *version);
method_t parse_method_enum(char *method_str);
boolean compare_strings(const char *a, const char *b, boolean ignore_case);
char* get_method_str(method_t method);
int has_multiple_segments(const char *endpoint);
char** split_endpoint_segments(const char *endpoint);


int get_strlen(const char* str);
concat_helper_t* str_concat(const char *a, const char *b);
int starts_with(const char *str, const char *prefix);
char* mv_stringtosubstring(const char* str, char* substr);
char* mv_stringto(char* string, char point);
char* strcopy_start_end(char* string, char startpoint, char endpoint);
char* find_closing_brace(char* start);
char* strndup(const char *str, size_t len);