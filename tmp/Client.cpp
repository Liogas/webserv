#include "Client.hpp"

Client::Client(int fd) : _fd(fd)
{
    std::cout << "New client connected : " << this->_fd << std::endl;
}