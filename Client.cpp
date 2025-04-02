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
        strerror(errno);
        throw Client::ErrorClient();
    }
    this->_contentLength = -1;
    this->_buffer = "";
    this->_bytes = 0;
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

void Client::resetBuffer(void)
{
    this->_buffer.resize(0);
}

bool Client::addBuffer(std::string buffer, ssize_t bytesRead)
{
    this->_buffer.append(buffer, this->_buffer.size(), bytesRead);
    this->_bytes += bytesRead;
    if (this->_contentLength == -1)
    {
        size_t startBody = this->_buffer.find("\r\n\r\n");
        if (startBody != std::string::npos)
        {
            size_t posContentLength = this->_buffer.find("Content-Length:");
            if (posContentLength != std::string::npos)
            {
                posContentLength += 15;
                size_t end = this->_buffer.find("\r\n", posContentLength);
                if (end != std::string::npos)
                {
                    std::stringstream ss( this->_buffer.substr(posContentLength, end - posContentLength));
                    ss >> this->_contentLength;
                    this->_bytes -= startBody + 4;
                }
            }
            else
            {
                std::cout << "GET METHOD" << std::endl;
                return (true);
            }
        }
    }
    if (this->_contentLength != -1 && this->_contentLength <= this->_bytes)
    {
        return (true);
    }
    return (false);
}

// GETTERS
int Client::getFd(void) const
{
    return (this->_fd);
}

std::string Client::getBuffer(void) const
{
    return (this->_buffer);
}

// EXCEPTIONS
const char *Client::ErrorClient::what() const throw()
{
    return ("Error on constructor Client");
}