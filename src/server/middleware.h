#pragma once
#include "server/http/request.h"
#include "types.h"

typedef struct server_t server_t;

#define ENDPOINTS(...) make_endpoint_array((const char*[]){__VA_ARGS__, NULL})
#define USE_MIDDLEWARE(s, fn, ...) \
    add_middleware(init_middleware(ENDPOINTS(__VA_ARGS__), fn), s)

typedef void (*middleware_callback_t)(http_request_t request);

typedef struct Middleware {
	middleware_callback_t callback;
	const char **endpoints;
} middleware_t;

middleware_t* init_middleware(const char **endpoints, middleware_callback_t cb);
boolean add_middleware(middleware_t *middleware_config, server_t *server_config);
const char** make_endpoint_array(const char** endpoint);
