#pragma once
#include <arpa/inet.h>
#include "http/request.h"
#include "http/response.h"
#include "router.h"
#include "middleware.h"
#include "types.h"

#define PORT(port_number) (htons(port_number))
#define BUFFER_LEN 2048

#define MAX_ROUTES 256

#define SET_IP(ip_str, addr_struct)                         \
    do {                                                    \
        if (inet_pton(AF_INET, (ip_str),                    \
                      &((addr_struct).sin_addr)) != 1) {    \
            perror("Invalid IP address");                   \
            exit(EXIT_FAILURE);                             \
        }                                                   \
    } while (0)

typedef struct client_t {
	address_t client_address; // 16 bytes;
	socklen_t client_address_length; // 16 bytes;
	socket_t client_socket; // 4 bytes
} client_t;

typedef struct server_t {
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

/**
 * Initializes the server with the given port and IP address.
 *
 * @param port The port number the OS will use to listen for incoming connections.
 * @param ip The IP address on which the server will run.
 * @return A pointer to a server_t structure containing socket and address information,
 *         or NULL if initialization fails.
 */
server_t* init_server(uint16_t port, const char* ip);
void start_server(server_t *server_config);