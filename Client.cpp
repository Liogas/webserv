#include "Client.hpp"

Client::Client(int fd, Server &serv) : _serv(serv), _fd(fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = this->_fd;
    if (epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_ADD, this->_fd, &event) == -1)
    {
        perror("epoll_ctl");
        throw Client::ErrorClient();
    }
    std::cout << "[Client] " << this->_fd << "created" << std::endl;
}

Client::~Client()
{
    std::cout << "[Client] " << this->_fd << " destroyed" << std::endl;
}

Request Client::readRequest(void)
{
    char buffer[1024] = {0};
    ssize_t bytes_read = read(this->_fd, buffer, 1024);
    buffer[bytes_read] = '\0';
    Request req(bytes_read, buffer);
    return (req);
}

void Client::disconnect(void)
{
    std::cout << "[Server] client " << this->_fd << " disconnected" << std::endl;
    close(this->_fd);
    epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_DEL, this->_fd, NULL);
}

void Client::sendResponse(std::string response)
{
    send(this->_fd, response.c_str(), response.size(), 0);
}

const char *Client::ErrorClient::what() const throw()
{
    return ("Error on constructor Client");
}