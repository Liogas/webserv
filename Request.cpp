#include "Request.hpp"

Request::Request(ssize_t bytes, char *buffer) : _bytes_read(bytes), _buffer(buffer, bytes)
{
    std::cout << "[Request created]" << std::endl;
}

Request::~Request(void)
{
    std::cout << "[Request] destroyed" << std::endl;
}

ssize_t Request::getBytesRead(void)
{
    return (this->_bytes_read);
}

std::string Request::getBuffer(void)
{
    return (this->_buffer);
}
