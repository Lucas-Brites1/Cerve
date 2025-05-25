#include "./server/server.h"
#include "./server/utils/utils.h"
#include "./server/json/json.h"
#include "./server/json/json_overload.h"
#include "./server/router.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

http_response_t hello(http_request_t req, http_response_t *res) {
    (void)req;

    res->body = "Hello world!\n";
    res->content_type = "text/plain";
    res->status_code = OK;
    return (*res);
}

http_response_t hellojson(http_request_t req, http_response_t *res) {
    (void)req;

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

    char* json = serialize(person);

    res->body = json;
    res->content_type = "application/json";
    res->status_code = 200;
    return (*res);
}

void logger(http_request_t request) {
    printf("\n\tMETHOD: %s\n\tENDPOINT: %s\n\tBODY: %s\n\t", get_method_str(request.method), request.endpoint, request.body);
    return;
}

typedef struct {
    char* street;
    char* city;
} addrrrrrr_t; 

typedef struct {
    char* name;
    int age;
    int empregado;
    void* adicionais;  
    addrrrrrr_t* endereco;  
    json_array_t* pets;
} person_info_t;  

    
/*
int main() {
    server_t* s = init_server(7979, LOCALHOST);
    if (!s) return 1;

    GET("/hello", hello, s);
    GET("/helloJSON", hellojson, s);
    USE_MIDDLEWARE(s, logger, "/hello", "/helloJSON");

    start_server(s);
    free(s);
    return 0;
}
    */


int main() {
    json_object_t* obj = create_json_object();
    JSON(obj, "nome", "Lucas Silva Brites");
    JSON(obj, "idade", 23);
    JSON_BOOLEAN(obj, "empregado", t);
    JSON_NULLISH(obj, "adicionais");

    json_object_t* address_obj = create_json_object();
    JSON(address_obj, "street", "Rua Exemplo");
    JSON(address_obj, "city", "Cidade Exemplo");
    JSON(obj, "endereco", address_obj);
    json_array_t* json_array = create_json_array();
    APPEND(json_array, "Zoe");
    JSON(obj, "pets", json_array);

    const char* json_string = serialize(obj);
    printf("JSON serializado:\n%s\n\n", json_string);

    field_descriptor_t address_fields[] = {
        DESERIALIZE_FIELD(FIELD_STRING, "street", addrrrrrr_t, street),
        DESERIALIZE_FIELD(FIELD_STRING, "city", addrrrrrr_t, city),
        END_FIELDS
    };

    field_descriptor_t persons[] = {
        DESERIALIZE_FIELD(FIELD_STRING, "nome", person_info_t, name),
        DESERIALIZE_FIELD(FIELD_NUMBER, "idade", person_info_t, age),
        DESERIALIZE_FIELD(FIELD_BOOL, "empregado", person_info_t, empregado),
        DESERIALIZE_FIELD(FIELD_NULL, "adicionais", person_info_t, adicionais),
        DESERIALIZE_FIELD_OBJECT(FIELD_OBJECT, "endereco", person_info_t, endereco, address_fields),
        DESERIALIZE_FIELD_ARRAY(FIELD_ARRAY, "pets", person_info_t, pets, ARRAY_STRING),
        END_FIELDS
    };

    person_info_t pessoa = {0};
    deserialize(json_string, &pessoa, persons);

    printf("Name: %s\n", pessoa.name);
    printf("Age: %d\n", pessoa.age);
    printf("Empregado: %d\n", pessoa.empregado);
    if (pessoa.adicionais == NULL) {
        printf("Adicionais não foram cadastrados\n");
    }
    if (pessoa.endereco) {
        printf("Endereço:\n");
        if(pessoa.endereco->city && pessoa.endereco->street) {
            printf("\tRua: %s\n", pessoa.endereco->street);
            printf("\tCidade: %s\n", pessoa.endereco->city);
        }
        else {
            printf("Deu algo errado...\n");
        }
        
    }

    return 0;
}