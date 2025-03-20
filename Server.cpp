#include "Server.hpp"

Server::Server(void) :
    _running(false),
    _config(NULL)
{
    if (!this->init())
        throw Server::ErrorSocket();
    std::cout << "[Server] created" << std::endl;
}

Server::Server(ServerConfig *config) :
    _running(false),
    _config(config)
{
    if (!this->init())
        throw Server::ErrorSocket();
    std::cout << "[Server " << this->_fd << "] created" << std::endl;
}

bool Server::init(void)
{
    this->_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    fcntl(this->_fd, F_SETFL, O_NONBLOCK);
    if (this->_fd == -1)
    {
        perror("socket");
        return false;
    }
    int reuse = 1;
    setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    return (true);
}

void Server::print(void)
{
    std::cout << "FD : " << this->_fd << std::endl;
    std::cout << "Running : " << this->_running << std::endl;
    this->_config->print();
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
    if (this->_config)
    {
        delete this->_config;
        this->_config = NULL;
    }
    close(this->_fd);
    std::cout << "[Server " << this->_fd << "] closed" << std::endl;
}

void Server::bindSocket(int maxClient)
{
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(this->_config->getPort());
    if (bind(this->_fd,
            reinterpret_cast<struct sockaddr*>(&this->_address),
            sizeof(this->_address)) == -1)
    {
        perror("bind");
        throw Server::ErrorBind();
    }
    if (listen(this->_fd, maxClient) == -1)
    {
        perror("listen");
        throw Server::ErrorBind();
    }
    std::cout << "[Server " << this->_fd << "] Listening on port "
        << ntohs(this->_address.sin_port) << std::endl;
}

bool Server::newClient(void)
{
    int client_fd = accept(this->_fd, NULL, NULL);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    if (client_fd == -1)
    {
        perror("accept");
        return (false);
    }
    try
    {
        Client *client = new Client(client_fd, *this);
        this->_clients.insert(std::make_pair(client_fd, client));
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (false);
    }
    std::cout << "[Server " << this->_fd << "] new client connected : " << client_fd << std::endl;
    return (true);
}

void Server::eraseClient(int clientFd)
{
    if (this->_clients.find(clientFd) != this->_clients.end())
        this->_clients.erase(clientFd);
    else
        std::cerr << "eraseClient : Client unknown" << std::endl;
}

void Server::setEpollFd(int fd)
{
    this->_epollFd = fd;
}

int Server::getFd(void) const
{
    return (this->_fd);
}

int Server::getEpollFd(void) const
{
    return (this->_epollFd);
}

std::map<int, Client *> Server::getClients(void) const
{
    return (this->_clients);
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
