#include "request.h"
#include <string.h>
#include <stdio.h>

void parse_request(Request* request_out, uint8_t* buffer, size_t buffer_len) {    
    uint8_t* method_text = strtok(buffer, " ");
    
    if (strcmp(method_text, "GET") == 0)
        request_out->method = METHOD_GET;
    else if (strcmp(method_text, "POST") == 0)
        request_out->method = METHOD_POST;
    else if (strcmp(method_text, "PUT") == 0)
        request_out->method = METHOD_PUT;
    else if (strcmp(method_text, "HEAD") == 0)
        request_out->method = METHOD_HEAD;

    request_out->route = strtok(NULL, " ");
}
