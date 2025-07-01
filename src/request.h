#pragma once
#include "methods.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    Method method;
    const uint8_t* route;
} Request;

void parse_request(Request* request_out, uint8_t* buffer, size_t buffer_len);