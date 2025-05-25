#pragma once
#include "server.h"

void middleware_listener(server_t* server_config, const char* buffer, char* endpoint, char* method);
void route_listener(server_t* server_config, const char* buffer, char* endpoint, char* method);
const char* get_status_reason_phrase(STATUS_CODE code);
void print_start(server_t *server_config);

void send_3XX(socket_t client_socket, STATUS_CODE code, const char* location);
void send_4XX(socket_t client_socket, STATUS_CODE code);
void send_5XX(socket_t client_t, STATUS_CODE code);
void send_final_response(socket_t client_socket, STATUS_CODE code, http_response_t* resp);