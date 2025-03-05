#include "Request.hpp"

Request::Request(ssize_t bytes, char *buffer) : _bytes_read(bytes), _buffer(buffer)
{
    std::cout << "[Request created] bytes=" << bytes << " ; buffer=" << buffer << std::endl;
}

Request::~Request(void)
{
    std::cout << "[Request] destroyed" << std::endl;
}

ssize_t Request::getBytesRead(void)
{
    return (this->_bytes_read);
}

char *Request::getBuffer(void)
{
    return (this->_buffer);
}
