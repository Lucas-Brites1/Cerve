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
#include "server/server_utils.h"

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
	print_start(server_config);

	server_config->client.client_address_length = sizeof(server_config->client.client_address);

	while(1) {
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
		
		middleware_listener(server_config, buffer, endpoint, method);

		route_listener(server_config, buffer, endpoint, method);

		close(server_config->client.client_socket);
	}

	close(server_config->server_socket);
}