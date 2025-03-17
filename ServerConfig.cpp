/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:04:34 by glions            #+#    #+#             */
/*   Updated: 2025/03/17 12:10:21 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig(void) : _serverName(), _port(), _routes()
{
    std::cout << "[ServerConfig] created" << std::endl;
}

ServerConfig::ServerConfig(ServerConfig const &copy):
    _serverName(copy.getServerName()),
    _port(copy.getPort()),
    _clientMaxBody(copy.getClientMaxBody()),
    _errorPages(copy.getErrorPages()),
    _routes()
{
    std::map<std::string, Route *> tmp;
    for (std::map<std::string, Route *>::iterator it = tmp.begin();
        it != tmp.end(); it++)
    {
        Route *tmp = new Route(*(it->second));
        this->_routes[it->first] = tmp;
    }
}

ServerConfig::~ServerConfig()
{
    for (std::map<std::string, Route *>::iterator it = this->_routes.begin(); it != this->_routes.end(); it++)
    {
        delete it->second;
        std::cout << "Route " << it->first << " destroyed" << std::endl;
    }
    std::cout << "[ServerConfig] destroyed" << std::endl;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &copy)
{
    std::cout << "[ServerConfig] copy assigment called" << std::endl;
    if (this != &copy)
    {
        this->_port = copy.getPort();
        this->_serverName = copy.getServerName();
        this->_routes = copy.getRoutes();
        this->_clientMaxBody = copy.getClientMaxBody();
    }
    return (*this);
}

void ServerConfig::setServerName(std::string name)
{
    this->_serverName = name;
}

void ServerConfig::setServerName(std::vector<std::string> args)
{
    if (args.size() > 2)
    {
        throw ServerConfig::ErrorToManyArgs();
    }
    else if (args.size() < 2)
    {
        throw ServerConfig::ErrorNotEnoughArgs();
    }
    this->_serverName = args[1];
}

void ServerConfig::setPort(std::vector<std::string> args)
{
    if (args.size() > 2)
    {
        throw ServerConfig::ErrorToManyArgs();
    }
    else if (args.size() < 2)
    {
        throw ServerConfig::ErrorNotEnoughArgs();
    }
    for (size_t i = 0; i < args[1].size(); i++)
    {
        if (!isdigit(args[1].at(i)))
            throw ServerConfig::ErrorNotValidArgs();
    }
    std::istringstream iss(args[1]);
    int num;
    if (!(iss >> num))
        throw ServerConfig::ErrorNotValidArgs();
    this->_port = num;
}

void ServerConfig::setPort(int port)
{
    this->_port = port;
}

void ServerConfig::addErrorPage(std::vector<std::string> args)
{
    /*
        Verifier si le code d'erreur est valide
        Verifier si le path est valide
    */
    if (args.size() > 3)
        throw ServerConfig::ErrorToManyArgs();
    else if (args.size() < 3)
        throw ServerConfig::ErrorNotEnoughArgs();
    std::istringstream iss(args[1]);
    int num;
    if (!(iss >> num))
        throw ServerConfig::ErrorNotValidArgs();
    if (this->_errorPages.find(num) != this->_errorPages.end())
        throw ServerConfig::ErrorErrorPageAlreadyIn();
    this->_errorPages[num] = args[2];
}

void ServerConfig::setClientMaxBody(std::vector<std::string> args)
{
    if (args.size() > 2)
    {
        throw ServerConfig::ErrorToManyArgs();
    }
    else if (args.size() < 2)
    {
        throw ServerConfig::ErrorNotEnoughArgs();
    }
    if (args[1].size() == 1 && !isdigit(args[1].at(0)))
        throw ServerConfig::ErrorNotValidArgs();
    for (size_t i = 0; i < args[1].size(); i++)
    {
        if (!isdigit(args[1].at(i)) && ((args[1].at(i) != 'K' && args[1].at(i) != 'M' &&
            args[1].at(i) != 'G') || i != args[1].size() - 1))
            throw ServerConfig::ErrorNotValidArgs();
    }
    char u = args[1].at(args[1].size() - 1);
    if (!isdigit(u))
        args[1].resize(args[1].size() - 1);
    std::istringstream iss(args[1]);
    int num;
    if (!(iss >> num))
        throw ServerConfig::ErrorNotValidArgs();
    if (!isdigit(u))
    {
        switch (u)
        {
            case 'G' :
                num = num * 1024;
            case 'M' :
                num = num * 1024;
            case 'K' :
                num = num * 1024;
                break;
            default :
                std::cerr << "UNITE PAS TROUVE, PAS NORMAL" << std::endl;
        }
    }
    this->_clientMaxBody = num;
}

void ServerConfig::addRoute(Route *route)
{
    this->_routes.insert(std::make_pair(route->getPath(), route));
}

std::string ServerConfig::getServerName(void) const
{
    return (this->_serverName);
}

size_t ServerConfig::getClientMaxBody(void) const
{
    return (this->_clientMaxBody);
}

int ServerConfig::getPort(void) const
{
    return (this->_port);
}
std::map<std::string, Route *> ServerConfig::getRoutes(void) const
{
    return (this->_routes);
}

std::map<int, std::string> ServerConfig::getErrorPages(void) const
{
    return (this->_errorPages);
}

const char *ServerConfig::ErrorNotEnoughArgs::what() const throw()
{
    return ("[!ServerConfig!] not enough args");
}

const char *ServerConfig::ErrorToManyArgs::what() const throw()
{
    return ("[!ServerConfig!] to much args");
}

const char *ServerConfig::ErrorNotValidArgs::what() const throw()
{
    return ("[!ServerConfig!] not valid args");
}

const char *ServerConfig::ErrorErrorPageAlreadyIn::what() const throw()
{
    return ("[!ServerConfig!] error pages already present");
}