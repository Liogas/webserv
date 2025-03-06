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

void Request::parseRequestLine(void)
{
    std::vector<std::string> lines = splitString(this->_buffer, '\n');
    if (lines.size() <= 0)
    {
        std::cerr << "[!Request!] lines.size <= 0" << std::endl;
        throw Request::ErrorParseRequestLine();  
    }
    std::vector<std::string> values = splitString(lines.at(0), ' ');
    if (values.size() != 3)
    {
        std::cerr << "[!Request!] values.size() != 3" << std::endl;
        throw Request::ErrorParseRequestLine();
    }
    if (values.at(0) == "GET")
        this->_request_method = GET;
    else if (values.at(0) == "POST")
        this->_request_method = POST;
    else if (values.at(0) == "DELETE")
        this->_request_method = DELETE;
    else
        this->_request_method = UNKNOWN;
    this->_request_path = values.at(1);
    this->_request_version = values.at(2);
    this->_request_version.erase(this->_request_version.find_last_not_of("\r") + 1);
}

void Request::verifRequestLine(void)
{
    bool error = false;
    if (this->_request_method == UNKNOWN)
    {
        std::cerr << "[!Request!] request method unknown (should be POST | DELETE | GET)" << std::endl;
        error = true;
    }
    if (this->_request_version != "HTTP/1.1")
    {
        std::cerr << "[!Request!] request http version invalid (sould be HTTP/1.1)" << std::endl;
        error = true;
    }
    if (error)
        throw Request::ErrorVerifRequestLine();
}

Method Request::getRequestMethod(void)
{
    return (this->_request_method);
}

std::string Request::getRequestPath(void)
{
    return (this->_request_path);
}

std::string Request::getRequestVersion(void)
{
    return (this->_request_version);
}

const char *Request::ErrorVerifRequestLine::what() const throw()
{
    return ("Error on verifRequestLine method");
}

const char *Request::ErrorParseRequestLine::what() const throw()
{
    return ("Error on parseRequestLine method");
}