#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "methods.h"
#include "request.h"
#include "response.h"

typedef void (*HandlerFunction)(Response* response);

typedef struct {
    const uint8_t* route_match;
    Method method;
    HandlerFunction handler_function;
} Handler;

typedef struct _HandlerListNode HandlerListNode;

// Linked list types
struct _HandlerListNode {
    Handler handler;
    HandlerListNode* next;
};

typedef struct {
    HandlerListNode* head;
} HandlerList;

bool get_matching_handler(Request request, HandlerList handlers, Handler* handler_out);