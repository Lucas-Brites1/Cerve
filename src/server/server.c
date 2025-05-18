#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server/server.h"
#include "./utils/utils.h"

server_t* init_server(uint16_t port, const char *ip) {
	server_t* s = (server_t*)malloc(sizeof(server_t));
	if(!s) return NULL;
	memset(s, 0, sizeof(server_t));
	
	s->server_address.sin_family = AF_INET;
	s->server_address.sin_port = htons(port);
	SET_IP(ip, s->server_address);
	memset(s->server_address.sin_zero, 0, sizeof(s->server_address.sin_zero));
	
	socket_t server_s = socket(AF_INET, SOCK_STREAM, 0);
	if(server_s < 0) {
		perror("Socket.\n");
		free(s);
		return NULL;
	}

	if (bind(server_s, (struct sockaddr*)&s->server_address, sizeof(s->server_address)) < 0) {
		perror("Bind");
		close(server_s);
		free(s);
		return NULL;
	}

	if (listen(server_s, 10) < 0) {
		perror("Listen");
		close(server_s);
		free(s);
		return NULL;
	}

	s->server_socket = server_s;

	return s;
}

void start_server(server_t *server_config) {
	if(!server_config) return;
	printf("Server listening at %s:%d!\n",inet_ntoa(server_config->server_address.sin_addr), ntohs(server_config->server_address.sin_port));
	server_config->client.client_address_length = sizeof(server_config->client.client_address);

	while(1) {
		boolean route_found = f;
		server_config->client.client_socket = accept(server_config->server_socket, (struct sockaddr*)&server_config->client.client_address, &server_config->client.client_address_length);

		if(server_config->client.client_socket < 0) {
			perror("Accept.\n");
			continue;
		}

		char buffer[BUFFER_LEN] = {0};
		ssize_t bytes_read = recv(server_config->client.client_socket, buffer, BUFFER_LEN - 1, 0);

		if (bytes_read < 0) {
			perror("Recv.\n");
			close(server_config->client.client_socket);
			continue;
		}

		char method[METHOD_LEN] = {0};
		char endpoint[ENDPOINT_LEN] = {0};
		char version[VERSION_LEN] = {0};

		parser(buffer, method, endpoint, version);
		
		for(int i=0; i<server_config->server_middlewares.middleware_counter; i++) {
			middleware_t *current_middleware = &server_config->server_middlewares.middlewares[i];
			if (current_middleware && (current_middleware->endpoints == NULL || current_middleware->endpoints[0] == NULL)) {
    			current_middleware->callback(create_http_request(buffer, endpoint, parse_method_enum(method)));
			}
			else if (current_middleware && current_middleware->endpoints!=NULL) {
				int j = 0;
				while(current_middleware->endpoints[j] != NULL) {
					if(compare_strings(current_middleware->endpoints[j], endpoint, f)) {
						current_middleware->callback(create_http_request(buffer, endpoint, parse_method_enum(method)));
					}
					j++;
				}
			}
		}

		for(int i=0; i<server_config->server_routes.route_counter; i++) {
			route_t *route = &server_config->server_routes.routes[i];
			if(route->handler == NULL) continue;

			if (compare_strings(route->method, method, f) && compare_strings(route->endpoint, endpoint, f)) {
				http_request_t http_req = create_http_request(buffer, endpoint, parse_method_enum(method));
				http_response_t* http_resp = (http_response_t*)malloc(sizeof(http_resp));

				http_response_t response = route->handler(http_req, http_resp);
				char response_buffer[BUFFER_LEN];
				int response_length = snprintf(response_buffer, BUFFER_LEN,
									"HTTP/1.1 %d OK\r\n"
									"Content-Type: %s\r\n"
									"Content-Length: %lu\r\n"
									"Connection: close\r\n"
									"\r\n"
									"%s",
									response.status_code,
									response.content_type,
									strlen(response.body),
									response.body
								);


				send(server_config->client.client_socket, response_buffer, response_length, 0);
				route_found = t;
				break;
			}
		}

		if (!route_found) {
    		char not_found[] = 
        		"HTTP/1.1 404 Not Found\r\n"
        		"Content-Type: text/plain\r\n"
        		"Content-Length: 13\r\n"
        		"Connection: close\r\n"
        		"\r\n"
        		"404 Not Found";
    		send(server_config->client.client_socket, not_found, strlen(not_found), 0);
		}

		close(server_config->client.client_socket);
	}
}