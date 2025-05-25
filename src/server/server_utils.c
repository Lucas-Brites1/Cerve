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

void middleware_listener(server_t* server_config, const char* buffer, char* endpoint, char* method) {
    for (int i = 0; i < server_config->server_middlewares.middleware_counter; i++) {
        middleware_t *current_middleware = &server_config->server_middlewares.middlewares[i];

        if (!current_middleware) continue;

        if (current_middleware->endpoints == NULL || current_middleware->endpoints[0] == NULL) {
            current_middleware->callback(create_http_request(buffer, endpoint, parse_method_enum(method)));
        } else {
            for (int j = 0; current_middleware->endpoints[j] != NULL; j++) {
                if (compare_strings(current_middleware->endpoints[j], endpoint, f)) {
                    current_middleware->callback(create_http_request(buffer, endpoint, parse_method_enum(method)));
                }
            }
        }
    }
}

void route_listener(server_t* server_config, const char* buffer, char* endpoint, char* method) {
    http_response_t* http_resp = malloc(sizeof(http_response_t));
    http_request_t http_req = create_http_request(buffer, endpoint, parse_method_enum(method));
    http_resp->body="Not Found";
    http_resp->status_code=404;
    http_resp->content_type = "text/plain";

    for (int i = 0; i < server_config->server_routes.route_counter; i++) {
        route_t *route = &server_config->server_routes.routes[i];
        if (!route->handler) continue;

        if (compare_strings(route->method, method, f) && compare_strings(route->endpoint, endpoint, f)) {
            *http_resp = route->handler(http_req, http_resp);
            
            break;
        }
    }

    send_final_response(server_config->client.client_socket, http_resp->status_code, http_resp);
    free(http_resp);
}

void print_start(server_t *server_config) {
    printf("Server listening at %s:%d!\n",
           inet_ntoa(server_config->server_address.sin_addr),
           ntohs(server_config->server_address.sin_port));
}

void send_3XX(socket_t client_socket, STATUS_CODE code, const char* location) {
    char response[512];

    const char* reason = get_status_reason_phrase(code);
    if (!location) location = "/";

    int response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Location: %s\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, reason, location);

    send(client_socket, response, response_len, 0);
}

void send_4XX(socket_t client_socket, STATUS_CODE code) {
    char response[512];

    const char* body = get_status_reason_phrase(code);
    int body_length = strlen(body);

    int response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        code, body, body_length, body);

    send(client_socket, response, response_len, 0);
}

void send_5XX(socket_t client_socket, STATUS_CODE code) {
    char response[512];

    const char* reason = get_status_reason_phrase(code);
    int body_length = strlen(reason);

    int response_len = snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        code, reason, body_length, reason);

    send(client_socket, response, response_len, 0);
}

void send_final_response(socket_t client_socket, STATUS_CODE code, http_response_t* resp) {
    if (code >= 300 && code < 400) {
        const char* location = (resp && resp->body) ? resp->body : "/";
        send_3XX(client_socket, code, location);
    }
    
    else if (code >= 400 && code < 500) send_4XX(client_socket, code);

    else if (code >= 500) send_5XX(client_socket, code);

    else {
        if (!resp || !resp->content_type || !resp->body) {
            send_5XX(client_socket, INTERNAL_SERVER_ERROR);
            return;
        }

        char response_buffer[BUFFER_LEN];
        int response_length = snprintf(response_buffer, BUFFER_LEN,
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            code,
            get_status_reason_phrase(code),
            resp->content_type,
            get_strlen(resp->body),
            resp->body
        );

        send(client_socket, response_buffer, response_length, 0);
    }
}

const char* get_status_reason_phrase(STATUS_CODE code) {
    switch (code) {
        // 1xx Informational
        case CONTINUE: return "Continue - The server has received the request and the client should continue with the request.";
        case SWITCHING_PROTOCOLS: return "Switching Protocols - The server is switching protocols as requested by the client.";
        case PROCESSING: return "Processing - The server has received the request but does not have a response available yet.";
        case EARLY_HINTS: return "Early Hints - The server is sending preliminary information before the final response.";

        // 2xx Success
        case OK: return "OK - The request was successful and the server has returned the requested data.";
        case CREATED: return "Created - The request was successful and a new resource has been created.";
        case ACCEPTED: return "Accepted - The request has been accepted for processing, but the processing has not been completed.";
        case NON_AUTHORITATIVE_INFORMATION: return "Non-Authoritative Information - The server is returning information that is not definitive.";
        case NO_CONTENT: return "No Content - The server processed the request but did not return any content.";
        case RESET_CONTENT: return "Reset Content - The server instructs the client to reset the document view.";
        case PARTIAL_CONTENT: return "Partial Content - The server is returning part of the requested content (used with the 'Range' header).";
        case MULTI_STATUS: return "Multi-Status - The server is returning multiple status codes for a single request.";
        case ALREADY_REPORTED: return "Already Reported - The response has already been reported and will not be repeated.";
        case IM_USED: return "IM Used - The server used the 'IM' (Instant Messaging) method for the request.";

        // 3xx Redirection
        case MULTIPLE_CHOICES: return "Multiple Choices - The requested resource has multiple options, the client should choose one.";
        case MOVED_PERMANENTLY: return "Moved Permanently - The requested resource has been permanently moved to a new location.";
        case FOUND: return "Found - The requested resource has been temporarily moved to a new location.";
        case SEE_OTHER: return "See Other - The client must go to another endpoint to get the response.";
        case NOT_MODIFIED: return "Not Modified - The resource has not been modified since the last request.";
        case TEMPORARY_REDIRECT: return "Temporary Redirect - The resource has been temporarily moved to a new URL.";
        case PERMANENT_REDIRECT: return "Permanent Redirect - The resource has been permanently moved to a new URL.";

        // 4xx Client Errors
        case BAD_REQUEST: return "Bad Request - The server cannot process the request due to a syntax error.";
        case UNAUTHORIZED: return "Unauthorized - The request requires user authentication.";
        case PAYMENT_REQUIRED: return "Payment Required - The request cannot be processed without payment (usually for premium services).";
        case FORBIDDEN: return "Forbidden - The server understood the request, but it refuses to authorize it.";
        case NOT_FOUND: return "Not Found - The requested resource could not be found on the server.";
        case METHOD_NOT_ALLOWED: return "Method Not Allowed - The HTTP method used is not allowed for the requested resource.";
        case NOT_ACCEPTABLE: return "Not Acceptable - The server cannot generate a response according to the requested media type.";
        case PROXY_AUTH_REQUIRED: return "Proxy Authentication Required - The client must authenticate with a proxy server.";
        case REQUEST_TIMEOUT: return "Request Timeout - The server did not receive a complete request within the time limit.";
        case CONFLICT: return "Conflict - The request could not be completed due to a conflict with the current state of the resource.";
        case GONE: return "Gone - The resource requested has been permanently removed and is no longer available.";
        case LENGTH_REQUIRED: return "Length Required - The server requires the content length to be specified in the request.";
        case PRECONDITION_FAILED: return "Precondition Failed - A precondition required by the request is not met.";
        case PAYLOAD_TOO_LARGE: return "Payload Too Large - The request's payload exceeds the server's size limits.";
        case URI_TOO_LONG: return "URI Too Long - The URI requested is longer than the server is willing to process.";
        case UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type - The server cannot process the media type of the request.";
        case RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable - The server cannot fulfill the range requested by the client.";
        case EXPECTATION_FAILED: return "Expectation Failed - The server cannot meet the expectations specified in the request.";
        case IM_A_TEAPOT: return "I'm a teapot - The server is a teapot and cannot brew coffee (a fictional error inspired by RFC 2324).";
        case MISDIRECTED_REQUEST: return "Misdirected Request - The request was sent to the wrong server and cannot be processed.";
        case UNPROCESSABLE_ENTITY: return "Unprocessable Entity - The server cannot process the entity of the request (usually due to invalid format).";
        case LOCKED: return "Locked - The requested resource is locked and cannot be modified.";
        case FAILED_DEPENDENCY: return "Failed Dependency - The request failed due to a failure in a previous dependent request.";
        case TOO_EARLY: return "Too Early - The server cannot process the request because it was sent too early.";
        case UPGRADE_REQUIRED: return "Upgrade Required - The client needs to upgrade to a newer protocol to complete the request.";
        case PRECONDITION_REQUIRED: return "Precondition Required - The server requires a specific condition to be met before processing the request.";
        case TOO_MANY_REQUESTS: return "Too Many Requests - The client has sent too many requests in a short period of time.";
        case REQUEST_HEADER_FIELDS_TOO_LARGE: return "Request Header Fields Too Large - The server cannot process the request because the headers are too large.";
        case UNAVAILABLE_FOR_LEGAL_REASONS: return "Unavailable For Legal Reasons - The resource is unavailable due to legal issues.";

        // 5xx Server Errors
        case INTERNAL_SERVER_ERROR: return "Internal Server Error - The server encountered an unexpected condition that prevented it from fulfilling the request.";
        case NOT_IMPLEMENTED: return "Not Implemented - The server does not support the method requested or the resource has not yet been implemented.";
        case BAD_GATEWAY: return "Bad Gateway - The server, while acting as a gateway or proxy, received an invalid response from the upstream server.";
        case SERVICE_UNAVAILABLE: return "Service Unavailable - The server is temporarily unable to handle the request due to maintenance or overload.";
        case GATEWAY_TIMEOUT: return "Gateway Timeout - The server, acting as a gateway or proxy, did not receive a timely response from the upstream server.";
        case HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported - The server does not support the HTTP version used in the request.";
        case VARIANT_ALSO_NEGOTIATES: return "Variant Also Negotiates - The server encountered an error while negotiating the content.";
        case INSUFFICIENT_STORAGE: return "Insufficient Storage - The server does not have enough storage to complete the request.";
        case LOOP_DETECTED: return "Loop Detected - The server detected an infinite loop while processing the request.";
        case NOT_EXTENDED: return "Not Extended - The server requires an extension to fulfill the request.";
        case NETWORK_AUTHENTICATION_REQUIRED: return "Network Authentication Required - Network authentication is required to access the resource.";

        default: return "Unknown Status - Unknown status code.";
    }
}

