#pragma once

typedef enum HTTP {
	GET = 0,
	POST = 1,
	DELETE = 2,
	UPDATE = 3,
	UNKOWN = 4
} method_t;


typedef struct {
	method_t method;
	const char *endpoint;
	const char *body;	
} http_request_t;

http_request_t create_http_request(const char* buffer, const char *endpoint, method_t method);
