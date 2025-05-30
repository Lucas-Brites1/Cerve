#include "utils.h"
#include "./server/http/request.h"
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

    int i = 0;
    while(str[i] != '\0') {
        str[i] = to_upper_char(str[i]);
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

concat_helper_t* str_concat(const char *a, const char *b) {
	if(!a || !b) return NULL;

    concat_helper_t* c = (concat_helper_t*) malloc(sizeof(concat_helper_t));
    if(!c) return NULL;

	int len_a = get_strlen(a);
	int len_b = get_strlen(b);

	char *result = (char *) malloc(sizeof(char) * (len_a + len_b + 1));
	if(!result) return NULL;

	int i=0;
	for(; i < len_a; i++) result[i] = a[i];
	for(int j=0; j < len_b; j++) result[i++] = b[j];
	result[i] = '\0';
    
    c->string_concatened=result;
    c->len = len_a + len_b + 1;
	return c;
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


http_request_t create_http_request(const char* buffer, const char *endpoint, method_t method) {
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
    } else {
        body_start = NULL;
    }
        http_request_t r = {0};
        r.body = body_start;
        r.method = method;
        r.endpoint = endpoint;
        return r;
}


int starts_with(const char *str, const char *prefix) {
	if(*prefix == '\0') return 1;
	if(*str != *prefix) return 0;
	return starts_with(str + 1, prefix + 1);
}

char* mv_stringtosubstring(const char* str, char* substr) {
	if((char)*str == '\0') return NULL;
	if(!str) return NULL;
	if(starts_with(str, substr)) return (char*)str;
	return mv_stringtosubstring(str + 1, substr);
}

char* mv_stringto(char* string, char point) {
    if(!string) return NULL; 
    if((char)*string == point) return string;
    if((char)*string == '\0') return NULL;
    return mv_stringto(string + 1, point);
}

char* strcopy_start_end(char* string, char startpoint, char endpoint) {
    if (!string) return NULL;

    char* start = mv_stringto(string, startpoint);
    if (!start) return NULL;
    start++;

    while (*start == ' ' || *start == ':') start++;

    char* end = start;

    if (*start == '\"') {
        start++; 
        end = start;
        while (*end && *end != '\"') end++;
    } else {
        while (*end && *end != endpoint && *end != '\n' && *end != '}' && *end != ']') end++;
        while (end > start && (*(end - 1) == ' ')) end--;
    }

    int len = end - start;
    char* new_string = malloc(len + 1);
    if (!new_string) return NULL;

    for (int i = 0; i < len; i++) new_string[i] = start[i];
    new_string[len] = '\0';

    return new_string;
}

char* find_closing_brace(char* start) {
    if (*start != '{') return NULL;  // segurança extra
    int brace_count = 1;
    start++;  // Avança para depois do primeiro '{'

    while (*start) {
        if (*start == '{') {
            brace_count++;
        } else if (*start == '}') {
            brace_count--;
            if (brace_count == 0) {
                return start;
            }
        }
        start++;
    }
    return NULL;
}

char* strndup(const char *str, size_t len) {
    char *dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, str, len);
        dup[len] = '\0';
    }
    return dup;
}
