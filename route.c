#include "server.h"
#include "utils.h"
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
    if(!server_config || !server_config->server_routes.routes) return f;

    if(server_config->server_routes.route_counter >= MAX_ROUTES) {
        fprintf(stderr, "Max routes reached: %d\n", MAX_ROUTES);
        return f;
    }

    server_config->server_routes.routes[server_config->server_routes.route_counter++] = *route_config;
    return t;
}

middleware_t* init_middleware(const char **endpoints, middleware_callback_t cb) {
    middleware_t* mcb = (middleware_t*)malloc(sizeof(middleware_t));
    mcb->callback = cb;
    mcb->endpoints = endpoints;

    return mcb;
}

boolean add_middleware(middleware_t *middleware_config, server_t *server_config) {
    if(!middleware_config || !server_config) return f;

    if(server_config->server_middlewares.middleware_counter >= MAX_ROUTES) {
        fprintf(stderr, "Max middlewares reached: %d\n", MAX_ROUTES);
        return f;
    }

    server_config->server_middlewares.middlewares[server_config->server_middlewares.middleware_counter++] = *middleware_config;
    return t;
}

const char** make_endpoint_array(const char** endpoints_input) {
    if (!endpoints_input) return NULL;

    int count = 0;
    while (endpoints_input[count] != NULL) {
        count++;
    }

    const char** result = malloc((count + 1) * sizeof(char*));
    if (!result) {
        perror("malloc failed");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        const char* ep = endpoints_input[i];

        int needs_slash = ep[0] != '/';
        int len = strlen(ep);

        char* formatted = malloc(len + needs_slash + 1);
        if (!formatted) {
            perror("malloc failed for endpoint");
            result[i] = NULL;
            continue;
        }

        if (needs_slash) {
            formatted[0] = '/';
            memcpy(formatted + 1, ep, len);
            formatted[len + 1] = '\0';
        } else {
            memcpy(formatted, ep, len + 1); // inclui '\0'
        }

        result[i] = formatted;
    }

    result[count] = NULL;
    return result;
}