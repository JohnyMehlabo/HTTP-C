#include "response.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#define __cplusplus__strings__
#include <strings.h>

void response_status_code(Response* response, uint32_t status_code) {
    response->status_code = status_code;
}

void response_set_body(Response* response, uint8_t* body, size_t body_len) {
    response->body = malloc(body_len);
    memcpy(response->body, body, body_len);
    response->body_len = body_len;
}

// Function to easily set a response's body to a file. The return value indicates if the action was succesful
bool response_set_body_from_file(Response* response, uint8_t* filepath, uint8_t* content_type) {
    FILE* file = fopen(filepath, "rb+");
    
    if (file == NULL)
        return false;
    
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    response->body = malloc(file_size);
    fread( response->body, file_size, 1, file);

    response_set_body(response, response->body, file_size);
    response_set_header(response, "content-type", content_type);

    fclose(file);
    return true;
}

extern char* strdup(const char*);

void response_set_header(Response* response, uint8_t* key, uint8_t* value) {
    // We create node and push it to the beginning of the linked list
    HeaderListNode* node = malloc(sizeof(HeaderListNode));
    node->header = (Header) {.key = strdup(key), .value = strdup(value)};
    node->next = response->headers.head;
    response->headers.head = node;

    response->total_header_len += strlen(key) + strlen(value) + 4;
}

uint8_t* serialize_response(const Response* response, size_t* len_out) {
    uint8_t* buffer = malloc(50 + response->body_len + response->total_header_len);
    sprintf(buffer, "HTTP/1.1 %d\n", response->status_code % 1000); // The number is calculated by % 1000 to ensure it has only 3 digits

    uint8_t pre_status_len = strlen(buffer);

    uint8_t* header_offset = buffer + pre_status_len;
    HeaderListNode* current_node = response->headers.head;
    while (current_node != NULL) {
        sprintf(header_offset, "%s: %s\r\n", current_node->header.key, current_node->header.value);
        header_offset = buffer + strlen(buffer);
        current_node = current_node->next;
    }

    sprintf(header_offset, "\n");

    uint8_t pre_body_len = strlen(buffer);

    if (response->body != NULL) {
        uint8_t* body_offset = buffer + pre_body_len;

        memcpy(body_offset, response->body, response->body_len);
    }
    
    *len_out = pre_body_len + response->body_len;
    // printf("Total size: %d\n", *len_out);
    return buffer;
}

void free_response(Response* response) {
    if (response->body != NULL) 
        free(response->body);

    // Free header data
    HeaderListNode* current_node = response->headers.head;
    while (current_node != NULL) {
        HeaderListNode* next_node = current_node->next;
        free(current_node->header.key);
        free(current_node->header.value);
        free(current_node);
        current_node = next_node;
    }
}