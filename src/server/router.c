#include "server/server.h"
#include "./utils/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

route_t init_route(const char *method, const char *endpoint, const char *version, route_handler_t handler) {
    route_t r;
    memset(&r, 0, sizeof(route_t));

    strncpy(r.method, method, METHOD_LEN - 1);
    strncpy(r.endpoint, endpoint, ENDPOINT_LEN - 1);
    strncpy(r.version, version, VERSION_LEN - 1);
    r.handler = handler;

    return r;
}

boolean add_route(route_t *route_config, server_t *server_config) {
    if(!server_config) return f;

    if(server_config->server_routes.route_counter >= MAX_ROUTES) {
        fprintf(stderr, "Max routes reached: %d\n", MAX_ROUTES);
        return f;
    }

    server_config->server_routes.routes[server_config->server_routes.route_counter++] = *route_config;
    return t;
}