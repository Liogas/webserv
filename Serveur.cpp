#include "Serveur.hpp"


Serveur::Serveur(void)
{
    this->_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->_fd == -1)
    {
        perror("socket");
        throw Serveur::ErrorSocket();
    }
    this->_running = false;
    int reuse = 1;
    setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    std::cout << "[Serveur] created" << std::endl;
}

Serveur::~Serveur(void)
{
    close(this->_fd);
    std::cout << "[Serveur] closed" << std::endl;
}

void Serveur::bindSocket(sa_family_t family, in_addr_t s_addr, int port)
{
    this->_address.sin_family = family;
    this->_address.sin_addr.s_addr = s_addr;
    this->_address.sin_port = htons(port);
    if (bind(this->_fd,
            reinterpret_cast<struct sockaddr*>(&this->_address),
            sizeof(this->_address)) == -1)
    {
        perror("bind");
        throw Serveur::ErrorBind();
    }
    std::cout << "[Serveur] Listening on port " << ntohs(this->_address.sin_port) << std::endl;
}

void Serveur::ready()
{
    if (listen(this->_fd, 10) == -1)
    {
        perror("listen");
        throw Serveur::ErrorReady();
    }
    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
    {
        perror("epoll_create1");
        throw Serveur::ErrorReady();
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = this->_fd;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_fd, &event) == -1)
    {
        perror("epoll_ctl");
        throw Serveur::ErrorReady();
    }
}

void Serveur::start(void)
{
    int nfds;
    this->_running = true;
    while (this->_running)
    {
        struct epoll_event events[10];
        nfds = epoll_wait(this->_epollFd, events, 10, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            throw Serveur::ErrorStart();
        }
        std::cout << "1" << std::endl;
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == this->_fd)
            {
                std::cout << "[Serveur] new client detected" << std::endl;
                this->acceptClient();
            }
            else
            {
                this->handleClient(events[i].data.fd);
            }
        }
    }
}

void Serveur::acceptClient(void)
{
    int client_fd = accept(this->_fd, NULL, NULL);
    if (client_fd == -1)
    {
        perror("accept");
        return ;
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = client_fd;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, client_fd, &event) == -1)
    {
        perror("epoll_ctl");
        throw Serveur::ErrorClient();
    }
    std::cout << "[Server] new client connected : " << client_fd << std::endl;
}

void Serveur::handleClient(int client_fd)
{
    std::cout << "Entree dans handleClient avec : " << client_fd << std::endl;
    char buffer[1024] = {0};
    ssize_t bytes_read = read(client_fd, buffer, 1024);
    if (bytes_read == 0)
    {
        std::cout << "[Server] client disconnected or error" << std::endl;
        close(client_fd);
        epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, client_fd, NULL);
        return ;
    }
    else if (bytes_read < 0)
    {
        perror("read");
        return ;
    }
    buffer[bytes_read] = '\0';
    // std::cout << "[Server] Received from client " << client_fd << ": " << buffer << std::endl;

    // Construire la réponse HTTP
    std::ostringstream oss;
    std::string response_body = "<html><body><h1>Bienvenue sur Webserv</h1></body></html>";
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: keep-alive\r\n";  // Ne ferme pas la connexion
    oss << response_body.size();
    response += "Content-Length: " + oss.str() + "\r\n";
    response += "\r\n";
    response += response_body;

    send(client_fd, response.c_str(), response.size(), 0);
}

const char *Serveur::ErrorSocket::what() const throw()
{
    return ("Error creation socket on serveur constructor");
}

const char *Serveur::ErrorBind::what() const throw()
{
    return ("Error on bindSocket method");
}

const char *Serveur::ErrorReady::what() const throw()
{
    return ("Error on ready method");
}

const char *Serveur::ErrorStart::what() const throw()
{
    return ("Error on start method");
}

const char *Serveur::ErrorClient::what() const throw()
{
    return ("Error on acceptClient method");
}
