#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

const std::string RESPONSE_HTML = "<!DOCTYPE html>\n<html>\n<head><title>Simple HTTP Server</title></head>\n<body><h1>Hello, World!</h1></body>\n</html>";

void* handleRequest(void* arg) {
    int socket = *(int*)arg;

// ルーティング
//     char buffer[1024] = {0};
//     read(socket, buffer, sizeof(buffer));
// 
//     std::string request(buffer);
// 
//     if (request.find("GET /") != std::string::npos) {
//         const std::string response = "Hello, world!";
//         std::stringstream ss;
//         ss << "HTTP/1.1 200 OK\r\n";
//         ss << "Content-Type: text/plain\r\n";
//         ss << "Content-Length: " << response.length() << "\r\n";
//         ss << "\r\n" << response;
//         send(socket, ss.str().c_str(), ss.str().length(), 0);
//     } else {
//         const std::string response = "Not Found";
//         std::stringstream ss;
//         ss << "HTTP/1.1 404 Not Found\r\n";
//         ss << "Content-Type: text/plain\r\n";
//         ss << "Content-Length: " << response.length() << "\r\n";
//         ss << "\r\n" << response;
//         send(socket, ss.str().c_str(), ss.str().length(), 0);
//     }

    std::stringstream ss;

    ss << "HTTP/1.1 200 OK\r\n";
    ss << "Content-Type: text/html\r\n";
    ss << "Content-Length: " << RESPONSE_HTML.length() << "\r\n";
    ss << "\r\n" << RESPONSE_HTML;

    send(socket, ss.str().c_str(), ss.str().length(), 0);
    close(socket);

    return nullptr;
}

class HttpServer {
public:
    HttpServer(int port) : port_(port) {}

    void start(bool is_auto_reconnection = false) {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind failed");

            if (is_auto_reconnection) {
                port_++;
                start();
            } else {
                exit(EXIT_FAILURE);
            }            
        }

        if (listen(server_fd, SOMAXCONN) < 0) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Server started, waiting for connections on port " << port_ << "...\n";

        while (true) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            pthread_t thread_id;
            pthread_create(&thread_id, nullptr, handleRequest, (void*)&new_socket);
            pthread_detach(thread_id);
        }
    }

private:
    int port_;
};

int main() {
    HttpServer server(14500);
    server.start();
    return 0;
}
