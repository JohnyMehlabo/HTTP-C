#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint8_t* key;
    uint8_t* value;
} Header;

typedef struct _HeaderListNode HeaderListNode;

struct _HeaderListNode {
    Header header;
    HeaderListNode* next;
};

typedef struct {
    HeaderListNode* head;
} HeaderList;

typedef struct {
    uint32_t status_code;

    uint8_t* body;
    uint32_t body_len;

    HeaderList headers;
    size_t total_header_len;
} Response;

void response_status_code(Response* response, uint32_t status_code);
void response_set_body(Response* response, uint8_t* body, size_t body_len);
bool response_set_body_from_file(Response* response, uint8_t* filepath, uint8_t* content_type);
void response_set_header(Response* response, uint8_t* key, uint8_t* value);

uint8_t* serialize_response(const Response* response, size_t* len_out);

void free_response(Response* response);