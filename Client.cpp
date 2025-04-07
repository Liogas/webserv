#include "Client.hpp"

Client::Client(int fd, Server &serv) : _serv(serv),
                                       _fd(fd),
                                       _parseReq(NULL)
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
    if (this->_parseReq)
        delete this->_parseReq;
}

void Client::disconnect(void)
{
    epoll_ctl(this->_serv.getEpollFd(), EPOLL_CTL_DEL, this->_fd, NULL);
    std::cout << "[Server " << this->_serv.getFd() << "] client disconnected : "
              << this->_fd << std::endl;
    close(this->_fd);
}

void Client::newParseRequest(std::string buffer, ssize_t bytes)
{
    this->_parseReq = new ParseRequest(buffer, bytes, &this->_serv, this);
}

void Client::updateParseRequest(std::string buffer, ssize_t bytes)
{
    this->_parseReq->addBuffer(buffer, bytes);
    this->_parseReq->setBytes(this->_parseReq->getBytes() + bytes);
}

void Client::deleteParseRequest()
{
    if (this->_parseReq)
    {
        delete this->_parseReq;
        this->_parseReq = NULL;
    }
}

int Client::parseRequest(void)
{
    std::string buff = this->_parseReq->getBuffer();
    size_t startBody = buff.find("\r\n\r\n");
    if (startBody != std::string::npos)
    {
        startBody += 4;
        this->_parseReq->setHeader(buff.substr(0, startBody));
        buff.erase(0, startBody);
        this->_parseReq->setBody(buff);
    }
    return (this->_parseReq->parseHeader());
}

int Client::requestReady(void)
{
    std::string buff = this->_parseReq->getBuffer();
    if (this->_parseReq->getContentLength() == -1)
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
                this->_parseReq->setContentLength(nb);
                this->_parseReq->setBytes(this->_parseReq->getBytes() - startBody + 4);
            }
            else
                return (this->parseRequest());
        }
    }
    if (this->_parseReq->getContentLength() != -1 &&
            this->_parseReq->getContentLength() <= this->_parseReq->getBytes())
    {
        std::cout << "Method POST recupérée" << std::endl;
        return this->parseRequest();
    }
    std::cout << "Return false" << std::endl;
    return (-1);
}

ParseRequest *Client::getParseReq(void) const
{
    return (this->_parseReq);
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