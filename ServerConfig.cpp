/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:04:34 by glions            #+#    #+#             */
/*   Updated: 2025/03/13 14:10:54 by glions           ###   ########.fr       */
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
    _routes(copy.getRoutes())
{
    if (this != &copy)
    {
        this->_port = copy.getPort();
        this->_serverName = copy.getServerName();
    }
}

ServerConfig::~ServerConfig()
{
    for (std::map<std::string, Route *>::iterator it = this->_routes.begin(); it != this->_routes.end(); it++)
    {
        std::cout << "Route " << it->first << " destroyed" << std::endl;
        delete it->second;
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
    }
    return (*this);
}

void ServerConfig::setServerName(std::string name)
{
    this->_serverName = name;
}

void ServerConfig::setPort(int port)
{
    this->_port = port;
}

void ServerConfig::addRoute(Route *route)
{
    this->_routes.insert(std::make_pair(route->getPath(), route));
}

std::string ServerConfig::getServerName(void) const
{
    return (this->_serverName);
}

int ServerConfig::getPort(void) const
{
    return (this->_port);
}
std::map<std::string, Route *> ServerConfig::getRoutes(void) const
{
    return (this->_routes);
}
