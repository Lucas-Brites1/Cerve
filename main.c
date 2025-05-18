#include "server.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

http_response_t hello(http_request_t req, http_response_t *res) {
    http_response_t r;
    (*res).body = "Hello world!\n";
    (*res).content_type = "text/plain";
    (*res).status_code = 200;
    return (*res);
}

http_response_t hellojson(http_request_t req, http_response_t *res) {
    http_response_t r;
    (*res).body = "{\"message\": \"Hello World, with JSON! :)\"}";
    (*res).content_type = "application/json";
    (*res).status_code = 200;
    return (*res);
}

void logger(http_request_t request) {
    printf("\n\tMETHOD: %s\n\tENDPOINT: %s\n\tBODY: %s\n\t", get_method_str(request.method), request.endpoint, request.body);
    return;
}

int main() {
    server_t* s = init_server(7979, "127.0.0.1");
    if (!s) return 1;

    GET("/hello", hello, s);
    GET("/helloJSON", hellojson, s);
    USE_MIDDLEWARE(s, logger, "/hello", "/helloJSON");

    start_server(s);
    free(s);
    return 0;
}