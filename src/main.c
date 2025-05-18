#include "./server/server.h"
#include "./server/utils/utils.h"
#include "./server/json/json.h"
#include "./server/json/json_overload.h"
#include "./server/router.h"
#include <stdio.h>
#include <stdlib.h>

http_response_t hello(http_request_t req, http_response_t *res) {
    (*res).body = "Hello world!\n";
    (*res).content_type = "text/plain";
    (*res).status_code = 200;
    return (*res);
}

http_response_t hellojson(http_request_t req, http_response_t *res) {
    json_object_t* obj = create_json_object();
    JSON(obj, "nome", "Lucas Silva Brites");
    JSON(obj, "idade", 23);
	JSON(obj, "ativo", f);
    JSON(obj, "altura", 1.78);
    char* json = json_stringfy(obj);
    puts(json);

    (*res).body = json;
    (*res).content_type = "application/json";
    (*res).status_code = 200;
    return (*res);
    free(json);
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