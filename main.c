#include "server.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

http_response_t hello(const char* body) {
    http_response_t r;
    r.body = "Hello world!\n";
    r.content_type = "text/plain";
    r.status_code = 200;
    return r;
}

http_response_t hellojson(const char* body) {
    http_response_t r;
    r.body = "{\"name\": \"Lucas\", \"age\": 23}";
    r.content_type = "application/json";
    r.status_code = 200;
    return r;
}

void logger(http_request_t request) {
    printf("\n\tMETHOD: %s\n\tENDPOINT: %s\n\tBODY: %s\n\t", get_method_str(request.method), request.endpoint, request.body);
    return;
}

int main() {
    server_t* s = init_server(7979, "127.0.0.1");
    if (!s) return 1;

    route_t route = init_route("GET", "/hello", "HTTP/1.1", hello);
    add_route(&route, s);

    route_t route2 = init_route("GET", "/hellojson", "HTTP/1.1", hellojson);
    add_route(&route2, s);

    add_middleware(init_middleware(make_endpoint_array("/hello/hellojson"), logger), s);

    start_server(s);
    free(s);
    return 0;
}