#pragma once
#include <stdint.h>
#include "handler.h"
#include "methods.h"

typedef struct _HTTPServer HTTPServer;

HTTPServer* create_server();
void close_server(HTTPServer* HTTPServer);

void register_handler(HTTPServer* server, const uint8_t* route_match, Method method, HandlerFunction handler_function);
void register_invalid_route_handler(HTTPServer* server, HandlerFunction handler_function);

void start_server(HTTPServer* server, uint16_t port);