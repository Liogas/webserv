/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 11:10:36 by glions            #+#    #+#             */
/*   Updated: 2025/04/07 15:08:12 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseRequest.hpp"

ParseRequest::ParseRequest(std::string buffer,
    ssize_t bytes, Server *serv, Client *client) :
    _header(),
    _body(),
    _buffer(buffer),
    _bytesRead(bytes),
    _copyRequest(),
    _saveRequest(),
    _route(NULL),
    _server(serv),
    _client(client)
{
    this->_headerInfo.accept = "";
    this->_headerInfo.acceptEncoding = "";
    this->_headerInfo.acceptLanguage = "";
    this->_headerInfo.agent = "";
    this->_headerInfo.boundary = "";
    this->_headerInfo.connection = "";
    this->_headerInfo.contentLength = -1;
    this->_headerInfo.contentType = "";
    this->_headerInfo.hostName = "";
    this->_headerInfo.transferEncoding = false;
    this->_headerInfo.url = "";
    this->_headerInfo.version = "";
}

ParseRequest::~ParseRequest()
{
    
}

void ParseRequest::addBuffer(std::string buffer, ssize_t bytes)
{
    this->_buffer.append(buffer, 0, bytes);
}

/* *********** PARSING *********** */

int ParseRequest::parseRequest(std::string request)
{
    std::stringstream ss(request);
    std::string token;
    
    while (ss >> token){
        if (token == "GET")
            this->_headerInfo.method = GET;
        else if (token == "POST")
            this->_headerInfo.method = POST;
        else if (token == "DELETE")
            this->_headerInfo.method = DELETE;
        else if (token[0] == '/')
            this->_headerInfo.url = token;
        else if (token == "HTTP/1.1")
            this->_headerInfo.version = token;
        else
            return (400);
    }
    if (this->_headerInfo.url.size() > 8192)
        return (414);
    this->_route = this->findLocation();
    if (this->_route == NULL)
        return (404);
    std::vector<Method> methods = this->_route->getMethods();
    std::vector<Method>::iterator it = std::find(methods.begin(), methods.end(),
        this->_headerInfo.method);
    if (it == this->_route->getMethods().end())
        return (405);
    if (this->_headerInfo.version != "HTTP/1.1")
        return (505);
    return (0);
}

int ParseRequest::parseHost(std::string host)
{
    std::vector<std::string> args = splitString(host, ' ');
    if (args.size() != 2)
        return (400);
    args = splitString(args[1], ':');
    if (args[0] != this->_server->getConfig()->getServerName())
        return (400);
    this->_headerInfo.hostName = args[0];
    return (0);
}

int ParseRequest::parseHeader(void)
{
    std::string line;
    std::istringstream stream(this->_header);
    int error;
    if (std::getline(stream, line) && (error = parseRequest(line)) != 0)
    {
        std::cerr << "ERROR REQUEST LINE -> " << error << std::endl;
        return (error);
    }
    while (std::getline(stream, line))
    {
        if (!line.compare(0, 5, "Host:") && (error = parseHost(line)) != 0)
            return (error);
        else if (!line.compare(0, 7, "Accept:"))
            this->_headerInfo.accept = line.substr(8);
        else if (!line.compare(0, 11, "User-Agent:"))
            this->_headerInfo.agent = line.substr(12);
        else if (!line.compare(0, 11, "Connection:"))
            this->_headerInfo.connection = line.substr(12);
        else if (!line.compare(0, 18, "Transfer-Encoding:"))
            this->_headerInfo.transferEncoding = true;
        else if (!line.compare(0, 16, "Accept-Language:"))
            this->_headerInfo.acceptLanguage = line.substr(17); 
        else if (!line.compare(0, 16, "Accept-Encoding:"))
            this->_headerInfo.acceptEncoding = line.substr(17);
        else if (!line.compare(0, 13, "Content-Type:"))
        {
            std::vector<std::string> args = splitString(line, ';');
            this->_headerInfo.contentType = args[0].substr(14, args[1].size() - 1);
            if (args.size() == 2)
                this->_headerInfo.boundary = args[1].substr(10);
        }
    }
    if (this->_headerInfo.transferEncoding && this->_headerInfo.contentLength != -1)
        return (400);
    return (0);
}

Route *ParseRequest::findLocation(void){
    std::string copyRequest;

    this->_copyRequest = this->_headerInfo.url;
    std::vector<std::string> rootToken = splitString(this->_headerInfo.url, '/');
    std::map<std::string, Route *> tmp = this->_server->getConfig()->getRoutes();
    std::map<std::string, Route*>::iterator it;
    it = tmp.end();
    while (it == tmp.end()){
        it = tmp.find(this->_copyRequest);
        if (it != tmp.end() && it->first == this->_copyRequest)
            break ;
        if (rootToken.size() <= 0)
            break ;
        std::string tmp;
        for (size_t i = 0; i < rootToken.size() - 1; ++i)
            tmp += rootToken[i];
        this->_copyRequest = tmp;
        this->_saveRequest.insert(0, rootToken[rootToken.size() - 1]);
        rootToken.pop_back();
    }
    if (it == tmp.end())
        return (NULL);
    return (it->second);
}

std::string ParseRequest::getBody(void) const
{
    return (this->_body);
}

Route   *ParseRequest::getRoute(void) const
{
    return (this->_route);
}

std::string ParseRequest::getBuffer(void) const
{
    return (this->_buffer);
}

int ParseRequest::getContentLength(void) const
{
    return (this->_headerInfo.contentLength);
}

int ParseRequest::getBytes(void) const
{
    return (this->_bytesRead);
}

t_header    ParseRequest::getHeaderInfo(void) const
{
    return (this->_headerInfo);
}

Server  *ParseRequest::getServer(void) const
{
    return (this->_server);
}

Client  *ParseRequest::getClient(void) const
{
    return (this->_client);
}

std::string ParseRequest::getSaveRequest(void) const
{
    return (this->_saveRequest);
}

void ParseRequest::setBytes(int value)
{
    this->_bytesRead = value;
}

void ParseRequest::setContentLength(int value)
{
    this->_headerInfo.contentLength = value;
}

void ParseRequest::setHeader(std::string header)
{
    this->_header = header;
}

void ParseRequest::setBody(std::string body)
{
    this->_body = body;
}