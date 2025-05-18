#pragma once
#include "http/request.h"
#include "http/response.h"
#include "types.h"

typedef struct server_t server_t;

#define METHOD_LEN 16
#define ENDPOINT_LEN 256
#define VERSION_LEN 16

#define GET(endpoint, handler, serverconfig)                 	\
    do {                                                     	\
        route_t* r = malloc(sizeof(route_t));                	\
        *r = init_route("GET", endpoint, "HTTP/1.1", handler); 	\
        add_route(r, serverconfig);                          	\
    } while(0)													\
	

typedef http_response_t (*route_handler_t)(http_request_t req, http_response_t *res);
    
typedef struct Route {
	route_handler_t handler; // 8
	char method[METHOD_LEN]; // 16 
	char endpoint[ENDPOINT_LEN]; // 256 
	char version[VERSION_LEN]; // 16
	// 296 bytes
} route_t;

route_t init_route(const char *method, const char *endpoint, const char *version, route_handler_t cb);
boolean add_route(route_t *route_config, server_t *server_config);