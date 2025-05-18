#pragma once

typedef struct {
	int status_code;
	const char* content_type;
	const char* body;
} http_response_t;
