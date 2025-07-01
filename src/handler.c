#include "handler.h"
#include <string.h>

bool get_matching_handler(Request request, HandlerList handlers, Handler* handler_out) {
    HandlerListNode* current_node = handlers.head;
    while (current_node != NULL) {
        if (current_node->handler.method == request.method && strcmp(current_node->handler.route_match, request.route) == 0) {
            *handler_out = current_node->handler;
            return true;
        } else {
            current_node = current_node->next;
        }
    }

    return false;
}