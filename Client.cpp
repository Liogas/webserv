#include "Client.hpp"

Client::Client(int fd, Server &serv) : _serv(serv),
                                       _fd(fd),
                                       _currReq(NULL)
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
    if (this->_currReq)
        delete this->_currReq;
}

void Client::disconnect(void)
{
    epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_DEL, this->_fd, NULL);
    std::cout << "[Server " << this->_serv.getFd() << "] client disconnected : "
              << this->_fd << std::endl;
    close(this->_fd);
}

void Client::newRequest(std::string buffer)
{
    this->_currReq = new Request(buffer, &this->_serv, this);
}

void Client::updateRequest(std::string buffer, ssize_t bytes)
{
    this->_currReq->addBuffer(buffer, bytes);
    this->_currReq->setBytes(this->_currReq->getBytes() + bytes);
}

void Client::deleteRequest()
{
    if (this->_currReq)
    {
        delete this->_currReq;
        this->_currReq = NULL;
    }
}

int Client::parseRequest(void)
{
    std::string buff = this->_currReq->getBuffer();
    size_t startBody = buff.find("\r\n\r\n");
    if (startBody != std::string::npos)
    {
        startBody += 4;
        this->_currReq->setHeader(buff.substr(0, startBody));
        buff.erase(0, startBody);
        this->_currReq->setBody(buff);
    }
    this->_currReq->parseHeader();
    return (0);
}

bool Client::requestReady(void)
{
    std::string buff = this->_currReq->getBuffer();
    if (this->_currReq->getContentLength2() != -1)
    {
        size_t startBody = buff.find("\r\n\r\n");
        if (startBody != std::string::npos)
        {
            size_t posContentLength = buff.find("Content-Length:");
            if (posContentLength != std::string::npos)
            {
                posContentLength += 15;
                size_t end = buff.find("\r\n", posContentLength);
                std::stringstream ss(buff.substr(posContentLength, end - posContentLength));
                int nb;
                ss >> nb;
                this->_currReq->setContentLength2(nb);
                this->_currReq->setBytes(this->_currReq->getContentLength2() - startBody + 4);
            }
            else
                return (true);
        }
    }
    if (this->_currReq->getContentLength2() != -1 &&
        this->_currReq->getContentLength2() <= this->_currReq->getBytes())
        return (true);
    return (false);
}

Request *Client::getCurrReq(void) const
{
    return (this->_currReq);
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
                    std::stringstream ss(this->_buffer.substr(posContentLength, end - posContentLength));
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