#include "./server/server.h"
#include "./server/utils/utils.h"
#include "./server/json/json.h"
#include "./server/json/json_overload.h"
#include "./server/router.h"
#include <stdio.h>
#include <stdlib.h>

http_response_t hello(http_request_t req, http_response_t *res) {
    res->body = "Hello world!\n";
    res->content_type = "text/plain";
    res->status_code = OK;
    return (*res);
}

http_response_t hellojson(http_request_t req, http_response_t *res) {
    json_object_t* person = create_json_object();
    JSON(person, "name", "Lucas Silva Brites");
    JSON(person, "age", 23);
	JSON_BOOLEAN(person, "employed", f);
    JSON(person, "height", 1.78);
    JSON_NULLISH(person ,"email");

    json_object_t*  pets = create_json_object();
    json_object_t* pet1 = create_json_object();
    json_array_t* arrpets = create_json_array();
    JSON(pet1, "name", "Zoe");
    JSON(pet1, "age", 1); 
    JSON(pets, "pet", pet1);
    APPEND(arrpets, pets);

    JSON(person, "pets", arrpets);

    char* json = json_stringify(person);

    res->body = json;
    res->content_type = "application/json";
    res->status_code = 200;
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