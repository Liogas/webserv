#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

#include "sendAnswer.hpp"
#include "cgi.hpp"

void sendResponse(int client_socket) {
    // Page HTML écrite en dur
    std::ostringstream oss;
    
        std::string html_content = cgi();
        std::cout << "===================> " << html_content << std::endl;
        oss << html_content.size();
    // Création de la réponse HTTP complète
    std::string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" + std::string(html_content);
        
    // Envoi de la réponse au client
    send(client_socket, http_response.c_str(), http_response.size(), 0);
}