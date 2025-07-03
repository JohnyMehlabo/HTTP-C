#include "httpc.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <netinet/ip.h>
#include "response.h"

struct _HTTPServer {
    HandlerList handlers;
    Handler invalid_route_handler;
};

HTTPServer* create_server() {
    HTTPServer* server = malloc(sizeof(HTTPServer));


    // Initialize list to be empty
    server->handlers.head = NULL;
    // Initialize invalid route handler to be invalid
    server->invalid_route_handler.handler_function = 0;

    return server;
}

void close_server(HTTPServer* HTTPServer) {
    HandlerListNode* current_node = HTTPServer->handlers.head;
    
    while (current_node != NULL) {
        HandlerListNode* next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }

    free(HTTPServer);
}

void register_invalid_route_handler(HTTPServer* server, HandlerFunction handler_function) {
    server->invalid_route_handler.handler_function = handler_function;
}

void register_handler(HTTPServer* server, const uint8_t* route_match, Method method, HandlerFunction handler_function) {
    // We create node and push it to the beginning of the linked list
    HandlerListNode* node = malloc(sizeof(HandlerListNode));
    node->handler = (Handler) {.route_match = route_match, .method = method, .handler_function = handler_function};
    node->next = server->handlers.head;
    server->handlers.head = node;
}

// Reads from socket file descriptor 'fd' until there are no more bytes left
// Returns a buffer that contains all the sent data. This buffer must be freed afterwards
uint8_t* receive_message(uint32_t fd, size_t* message_buffer_size) {
        *message_buffer_size = 0;
        size_t bytes_read;
        uint8_t chunk_buffer[1024];
        uint8_t* buffer = NULL;
        do {
            bytes_read = recv(fd, chunk_buffer, 1024, 0);
            *message_buffer_size += bytes_read;
            buffer = realloc(buffer, *message_buffer_size);
            memcpy(buffer + *message_buffer_size - bytes_read, chunk_buffer, bytes_read);
        } while (bytes_read == 1024);

        return buffer;
}

void start_server(HTTPServer* server, uint16_t port) {
    // Create our socket
    uint32_t socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Create our address object
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    // Bind the file descriptor to that address
    bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));

    // Start listening for connections
    listen(socket_fd, 64);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        uint32_t client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);

        // Receive complete message from the client
        size_t message_buffer_len;
        uint8_t* message_buffer = receive_message(client_fd, &message_buffer_len);

        // Parse incoming HTTP as request
        Request request;
        parse_request(&request, message_buffer, message_buffer_len);
        
        // Find matching handler for route and method
        Handler handler;
        if (!get_matching_handler(request, server->handlers, &handler)) {
            if (server->invalid_route_handler.handler_function != NULL) {
                handler = server->invalid_route_handler;
            } else {
                // printf("[ERROR] Invalid route and method combination. No matching handler. %s\n", request.route);
                // If there are no matches send error message and close connection
                send(client_fd, "HTTP/1.1 404\n\nInavlid route. ", 29, 0);
                close(client_fd);
                continue;
            }
        }

        // The response pointer should be assigned in the handler to the heap as it wil be freed
        Response response = {0}; 
        response.status_code = 200; // Default status code is 200
        // Call handler which will set the response information for us
        handler.handler_function(&response);
 
        // We serialize the response object into text. We free it afterwards as it is no longer used
        size_t response_raw_len;
        uint8_t* response_raw = serialize_response(&response, &response_raw_len);

        free_response(&response);

        // Send response
        send(client_fd, response_raw, response_raw_len, 0);
    
        // We free the raw response buffer
        free(response_raw);

        // Free the buffer containing the message
        free(message_buffer);
        
        // Close connection
        close(client_fd);
    }
    
}