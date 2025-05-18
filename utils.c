#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

boolean is_letter_lowercase(char letter) {
    return (letter >= 'a' && letter <= 'z') ? t : f; 
}

char to_upper_char(char c) {
    return is_letter_lowercase(c) ? c - 32 : c;
}

void to_upper(char *str) {
    if(!str) return;

    int i=0;
    while(str[i] != '\0') {
        if(is_letter_lowercase(str[i])) to_upper_char(str[i]);
        i++;
    }
}

void parser(char *buffer, char *method, char *endpoint, char *version) {
    if(!buffer || !method || !endpoint || !version) return;
    //GET /hello HTTP/1.1\r\n
    int i = 0, j = 0;

    while(buffer[i] != ' ' && buffer[i] != '\0') {
        method[j++] = buffer[i++];
    }

    method[j] = '\0';

    if (buffer[i] == '\0') return;
    j = 0;
    i++;

    while(buffer[i] != ' ' && buffer[i] != '\0') {
        endpoint[j++] = buffer[i++];
    }

    if (buffer[i] == '\0') return;
    endpoint[j] = '\0';
    j = 0;
    i++;

    while(buffer[i] != '\r' && buffer[i] != '\n' && buffer[i] != '\0') {
        version[j++] = buffer[i++];
    }
    version[j] = '\0';
}

int get_strlen(const char* str) {
    int i=0;
    while(str[i] != '\0') i++;
    return i;
}

char* str_concat(const char *a, const char *b) {
	if(!a || !b) return NULL;

	int len_a = get_strlen(a);
	int len_b = get_strlen(b);

	char *result = (char *) malloc(sizeof(char) * (len_a + len_b + 1));
	if(!result) return NULL;

	int i=0;
	for(; i < len_a; i++) result[i] = a[i];
	for(int j=0; j < len_b; j++) result[i++] = b[j];
	result[i] = '\0';

	return result;
}

boolean compare_strings(const char *a, const char *b, boolean ignore_case) {
    if (!a || !b) return f;

    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        char ca = ignore_case ? a[i] : to_upper_char(a[i]);
        char cb = ignore_case ? b[i] : to_upper_char(b[i]) ;
        if (ca != cb) return f;
        i++;
    }

    return (a[i] == '\0' && b[i] == '\0') ? t : f;
}

http_request_t create_http_request(method_t method, const char *endpoint, const char *version, const char *body) {
    return (http_request_t){
        .method = method,
        .endpoint = endpoint,
        .version = version,
        .body = body
    };
}

method_t parse_method_enum(char *method_str) {
    if(method_str[0] == 'G' || method_str[0] == 'g') return GET;

    if(compare_strings(method_str, "GET", f)) return GET;
    else if(compare_strings(method_str, "POST", f)) return POST;
    else if(compare_strings(method_str, "UPDATE", f)) return UPDATE;
    else if(compare_strings(method_str, "DELETE", f)) return DELETE;
    else return UNKOWN;
}

char* get_method_str(method_t method) {
    switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case UPDATE:
            return "UPDATE";
        case DELETE:
            return "DELETE";
        default:
            return "UNKNOW";
    }
}

int has_multiple_segments(const char *endpoint) {
    if(!endpoint) return 0;
    int count = 0;

    for(int i=0; endpoint[i] != '\0'; i++) if(endpoint[i] == '/') count++;

    return count > 1 ? count : 0;
}

char** split_endpoint_segments(const char *endpoint) {
    int segment_counter = has_multiple_segments(endpoint);
    if(!segment_counter) return NULL;

    int count = 0;
    char** segments = (char**)malloc(segment_counter * sizeof(char*));
    int len = get_strlen(endpoint);
    int i = 0;
    //"/hello/hellojson"
    while(i < len) {
        while(endpoint[i] == '/' && i < len) i++;

        if(i >= len) break;

        int start = i;

        while(endpoint[i] != '/' && i < len) i++;

        int segment_len = i - start;

        char *segment = malloc(segment_len + 1);
        memcpy(segment, &endpoint[start], segment_len);
        segment[segment_len] = '\0';
        segments[count++] = segment;
    }

    return segments;
}