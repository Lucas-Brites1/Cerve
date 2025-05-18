#pragma once
#include <arpa/inet.h>

#define PORT(port_number) (htons(port_number))
#define BUFFER_LEN 2048
#define METHOD_LEN 16
#define ENDPOINT_LEN 256
#define VERSION_LEN 16
#define MAX_ROUTES 256


#define SET_IP(ip_str, addr_struct)                         \
    do {                                                    \
        if (inet_pton(AF_INET, (ip_str),                    \
                      &((addr_struct).sin_addr)) != 1) {    \
            perror("Invalid IP address");                   \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)


#define GET(endpoint, handler, serverconfig)                 	\
    do {                                                     	\
        route_t* r = malloc(sizeof(route_t));                	\
        *r = init_route("GET", endpoint, "HTTP/1.1", handler); 	\
        add_route(r, serverconfig);                          	\
    } while(0)													\
	

#define ENDPOINTS(...) make_endpoint_array((const char*[]){__VA_ARGS__, NULL})
#define USE_MIDDLEWARE(s, fn, ...) \
    add_middleware(init_middleware(ENDPOINTS(__VA_ARGS__), fn), s)

typedef enum {
	t = 1,
	f = 0
} boolean;

typedef struct sockaddr_in address_t;
typedef int socket_t;

typedef enum HTTP {
	GET = 0,
	POST = 1,
	DELETE = 2,
	UPDATE = 3,
	UNKOWN = 4
} method_t;

typedef struct {
	int status_code;
	const char* content_type;
	const char* body;
} http_response_t;

typedef struct {
	method_t method;
	const char *endpoint;
	const char *body;	
} http_request_t;

typedef http_response_t (*route_handler_t)(http_request_t req, http_response_t *res);
typedef void (*middleware_callback_t)(http_request_t request);

typedef struct Middleware {
	middleware_callback_t callback;
	const char **endpoints;
} middleware_t;

typedef struct Route {
	route_handler_t handler; // 8
	char method[METHOD_LEN]; // 16 
	char endpoint[ENDPOINT_LEN]; // 256 
	char version[VERSION_LEN]; // 16
	// 296 bytes
} route_t;

typedef struct Client {
	address_t client_address; // 16 bytes;
	socklen_t client_address_length; // 16 bytes;
	socket_t client_socket; // 4 bytes
} client_t;

typedef struct Server {
	struct {
		route_t routes[MAX_ROUTES]; // 256 * 296 = ~74KB
		int route_counter; // 4 bytes
	} server_routes;

	struct server_middlewares {
		middleware_t middlewares[MAX_ROUTES];
		int middleware_counter;
	} server_middlewares;

	client_t client;
	address_t server_address; // sockaddr_in = 16bytes
	socket_t server_socket; // 4 bytes
} server_t;


server_t* init_server(uint16_t port, const char* ip);
void start_server(server_t *server_config);

route_t init_route(const char *method, const char *endpoint, const char *version, route_handler_t cb);
boolean add_route(route_t *route_config, server_t *server_config);

middleware_t* init_middleware(const char **endpoints, middleware_callback_t cb);
boolean add_middleware(middleware_t *middleware_config, server_t *server_config);
const char** make_endpoint_array(const char** endpoint);
