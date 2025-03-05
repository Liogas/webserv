#include "Server.hpp"


Server::Server(void)
{
    this->_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->_fd == -1)
    {
        perror("socket");
        throw Server::ErrorSocket();
    }
    this->_running = false;
    int reuse = 1;
    setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    std::cout << "[Server] created" << std::endl;
}

Server::~Server(void)
{
    if (this->_clients.size() > 0)
    {
        for (std::map<int, Client*>::iterator it = this->_clients.begin();
            it != this->_clients.end(); ++it)
        {
            it->second->disconnect();
            delete it->second;
        }
        this->_clients.erase(this->_clients.begin(), this->_clients.end());
    }
    close(this->_fd);
    std::cout << "[Server] closed" << std::endl;
}

void Server::bindSocket(sa_family_t family, in_addr_t s_addr, int port)
{
    this->_address.sin_family = family;
    this->_address.sin_addr.s_addr = s_addr;
    this->_address.sin_port = htons(port);
    if (bind(this->_fd,
            reinterpret_cast<struct sockaddr*>(&this->_address),
            sizeof(this->_address)) == -1)
    {
        perror("bind");
        throw Server::ErrorBind();
    }
    std::cout << "[Server] Listening on port " << ntohs(this->_address.sin_port) << std::endl;
}

void Server::ready()
{
    if (listen(this->_fd, 10) == -1)
    {
        perror("listen");
        throw Server::ErrorReady();
    }
    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
    {
        perror("epoll_create1");
        throw Server::ErrorReady();
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = this->_fd;
    if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, this->_fd, &event) == -1)
    {
        perror("epoll_ctl");
        throw Server::ErrorReady();
    }
}

void Server::start(void)
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
            throw Server::ErrorStart();
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == this->_fd)
            {
                std::cout << "[Server] new client detected" << std::endl;
                this->acceptClient();
            }
            else
            {
                this->handleClient(events[i].data.fd);
            }
        }
    }
}

void Server::acceptClient(void)
{
    int client_fd = accept(this->_fd, NULL, NULL);
    if (client_fd == -1)
    {
        perror("accept");
        return ;
    }
    try
    {
        Client *client = new Client(client_fd, *this);
        this->_clients.insert(std::make_pair(client_fd, client));
    } catch (std::exception &e)
    {
        return ;
    }
    std::cout << "[Server] new client connected : " << client_fd << std::endl;
}

void Server::handleClient(int client_fd)
{
    std::map<int, Client*>::iterator it = this->_clients.find(client_fd);
    if (it == this->_clients.end() || it->first != client_fd)
    {
        std::cout << "[Server] request from unknown client" << std::endl;
        return ;
    }
    Request req = it->second->readRequest();
    if (req.getBytesRead() == 0)
    {
        it->second->disconnect();
        delete it->second;
        this->_clients.erase(it);
        return ;
    }
    else if (req.getBytesRead() < 0)
    {
        perror("read");
        return ;
    }
    // req.getBuffer()[req.getBytesRead()] = '\0';
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
    it->second->sendResponse(response);
}

int Server::getEpollFd(void)
{
    return (this->_epollFd);
}

const char *Server::ErrorSocket::what() const throw()
{
    return ("Error creation socket on Server constructor");
}

const char *Server::ErrorBind::what() const throw()
{
    return ("Error on bindSocket method");
}

const char *Server::ErrorReady::what() const throw()
{
    return ("Error on ready method");
}

const char *Server::ErrorStart::what() const throw()
{
    return ("Error on start method");
}

const char *Server::ErrorClient::what() const throw()
{
    return ("Error on acceptClient method");
}
