#include "Server.hpp"

Server::Server(void)
{
    this->_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    fcntl(this->_fd, F_SETFL, O_NONBLOCK); 
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
        nfds = epoll_wait(this->_epollFd, events, 10, 0);
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
    fcntl(client_fd, F_SETFL, O_NONBLOCK); 
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
        std::cerr << e.what() << std::endl;
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

    try
    {
        req.parseRequestLine();
        std::cout << "infos sur la requete :" << std::endl;
        std::cout << "Method : " << req.getRequestMethod() << std::endl;
        std::cout << "Path : " << req.getRequestPath() << std::endl;
        std::cout << "Version : " << req.getRequestVersion() << std::endl;
        req.verifRequestLine();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return ;
    }
   
    std::vector<std::string> tmp = splitString(req.getRequestPath(), '/');
    std::cout << "Decoupage du path (size : " << tmp.size() << ") : " << std::endl;
    for (size_t i = 0; i < tmp.size(); i++)
        std::cout << "|" << tmp.at(i) << "|" << std::endl;
    
    /*
        REVOIR LE SYSTEME DE ROUTE POUR L'IMPLEMENTER ICI
    */

    // TEST POUR ENVOYER LE CONTENU D'UN FICHIER HTML
    std::ifstream file("index.html");
    if (!file)
    {
        std::cerr << "index.html not found" << std::endl;
        return ;
    }
    std::string line;
    std::string response_body;
    std::ostringstream oss;
    std::string response;
    while (std::getline(file, line))
        response_body += line;
    file.close();
    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: keep-alive\r\n";  // Ne ferme pas la connexion
    oss << response_body.size();
    response += "Content-Length: " + oss.str() + "\r\n";
    response += "\r\n";
    response += response_body;
    it->second->sendResponse(response);
}

void Server::addRoute(Route route)
{
    this->_routes.insert(std::make_pair(route.getPath(), route));
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
