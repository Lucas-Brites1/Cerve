#include "server.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

route_t init_route(const char *method, const char *endpoint, const char *version, route_callback_t cb) {
    route_t r;
    memset(&r, 0, sizeof(route_t));

    strncpy(r.method, method, METHOD_LEN - 1);
    strncpy(r.endpoint, endpoint, ENDPOINT_LEN - 1);
    strncpy(r.version, version, VERSION_LEN - 1);
    r.callback = cb;

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

const char** make_endpoint_array(const char* endpoint) {
    if(!endpoint) return NULL;
    if(has_multiple_segments(endpoint)) {
        char** segments = split_endpoint_segments(endpoint);
        if(!segments) return NULL;

        int count = 0;
        while(segments[count]) count++;

        const char** endpoints = malloc((count+1) * sizeof(char*));

        for(int i = 0; i < count; i++) {
            int len = get_strlen(segments[i]);

            char *fullendpoint = malloc(len + 2);
            if(!fullendpoint) continue;

            fullendpoint[0] = '/';   
            for(int j=0; j < len; j++) fullendpoint[j + 1] = segments[i][j];
            fullendpoint[len+1] = '\0';

            endpoints[i] = fullendpoint;
            free(segments[i]);
        }

        endpoints[count] = NULL;
        free(segments);
        return endpoints;
    }

    const char **endpoint_array = malloc(2 * sizeof(char*));
    if(!endpoint_array) return NULL;

    endpoint_array[0] = endpoint;
    endpoint_array[1] = NULL;
    return endpoint_array;
}
