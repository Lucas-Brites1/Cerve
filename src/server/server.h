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

typedef enum {
    // 1xx Informational
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    PROCESSING = 102,
    EARLY_HINTS = 103,

    // 2xx Success
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTI_STATUS = 207,
    ALREADY_REPORTED = 208,
    IM_USED = 226,

    // 3xx Redirection
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    TEMPORARY_REDIRECT = 307,
    PERMANENT_REDIRECT = 308,

    // 4xx Client Errors
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTH_REQUIRED = 407,
    REQUEST_TIMEOUT = 408,
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    IM_A_TEAPOT = 418,
    MISDIRECTED_REQUEST = 421,
    UNPROCESSABLE_ENTITY = 422,
    LOCKED = 423,
    FAILED_DEPENDENCY = 424,
    TOO_EARLY = 425,
    UPGRADE_REQUIRED = 426,
    PRECONDITION_REQUIRED = 428,
    TOO_MANY_REQUESTS = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    UNAVAILABLE_FOR_LEGAL_REASONS = 451,

    // 5xx Server Errors
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
    VARIANT_ALSO_NEGOTIATES = 506,
    INSUFFICIENT_STORAGE = 507,
    LOOP_DETECTED = 508,
    NOT_EXTENDED = 510,
    NETWORK_AUTHENTICATION_REQUIRED = 511
} STATUS_CODE;
