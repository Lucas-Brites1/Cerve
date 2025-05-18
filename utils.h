#pragma once
#include <stdio.h>
#include "server.h"

void parser(char *buffer, char *method, char *endpoint, char *version);
method_t parse_method_enum(char *method_str);
boolean compare_strings(const char *a, const char *b, boolean ignore_case);
http_request_t create_http_request(method_t method, const char *endpoint, const char *version, const char *body);
char* get_method_str(method_t method);
int has_multiple_segments(const char *endpoint);
char** split_endpoint_segments(const char *endpoint);
char* str_concat(const char *a, const char *b);
int get_strlen(const char* str);