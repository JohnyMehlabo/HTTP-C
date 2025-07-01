# HTTP-C
Simple http-only (no https) web server libary for C.

## Building
This library works exclusively on linux-based operating systems, as it uses native syscalls. 
To build just run:
```
make
```
And it will automatically generate the dynamic library (libhttpc.so)

## How to use
First a server must be created using ```create_server()```. Then route handlers are registerd with ```register_handler(server, route, method, handler)```. Once everything has been set-up a final call to  ```start_server(port)``` would start the server on the desired port.

## Example code
These library was made to be easy to use. Here is a code snippet that starts a server on port 3000 and serves a file ``index.html`` at ``/``
```C
#include <httpc.h>

void root_handler(Response* res) {
    if (!response_set_body_from_file(res, "index.html", "text/html")) {
        response_set_body(res, "Couldn't open file", 19);
        response_status_code(res, 500);
    }
}

int main() {
    HTTPServer* server = create_server();
    
    register_handler(server, "/", METHOD_GET, root_handler);

    start_server(server, 3000);
    close_server(server);
}
```
