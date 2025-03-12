/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:04:34 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 14:24:54 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig(void) : _serverName(), _port()
{
    std::cout << "[ServerConfig] created" << std::endl;
}

ServerConfig::ServerConfig(ServerConfig const &copy)
{
    if (this != &copy)
    {
        this->_port = copy.getPort();
        this->_serverName = copy.getServerName();
    }
}

ServerConfig::~ServerConfig()
{
    std::cout << "[ServerConfig] destroyed" << std::endl;
}

void ServerConfig::setServerName(std::string name)
{
    this->_serverName = name;
}

void ServerConfig::setPort(int port)
{
    this->_port = port;
}

std::string ServerConfig::getServerName(void) const
{
    return (this->_serverName);
}

int ServerConfig::getPort(void) const
{
    return (this->_port);
}

ServerConfig &ServerConfig::operator=(const ServerConfig &copy)
{
    std::cout << "[ServerConfig] copy assigment called" << std::endl;
    if (this != &copy)
    {
        this->_port = copy.getPort();
        this->_serverName = copy.getServerName();
    }
    return (*this);
}