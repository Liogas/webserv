#include "Client.hpp"

Client::Client(int fd, Server &serv) : 
    _serv(serv),
    _fd(fd)
{
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = this->_fd;
    if (epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_ADD, this->_fd, &event) == -1)
    {
        perror("epoll_ctl");
        throw Client::ErrorClient();
    }
}

Client::~Client()
{
}

void Client::disconnect(void)
{
    epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_DEL, this->_fd, NULL);
    std::cout << "[Server "<< this->_serv.getFd() << "] client disconnected : "
        << this->_fd << std::endl;
    close(this->_fd);
}

// GETTERS
int Client::getFd(void) const
{
    return (this->_fd);
}

// EXCEPTIONS
const char *Client::ErrorClient::what() const throw()
{
    return ("Error on constructor Client");
}