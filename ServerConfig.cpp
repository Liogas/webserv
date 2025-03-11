/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 11:04:34 by glions            #+#    #+#             */
/*   Updated: 2025/03/11 15:34:10 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig(void) : _server_names(), _ports()
{
    std::cout << "[ServerConfig] created" << std::endl;
}

ServerConfig::ServerConfig(ServerConfig const &copy)
{
    if (this != &copy)
    {
        this->_ports = copy.getPorts();
        this->_server_names = copy.getServerNames();
    }
}

ServerConfig::~ServerConfig()
{
    std::cout << "[ServerConfig] destroyed" << std::endl;
}

void ServerConfig::addServerName(std::string name)
{
    this->_server_names.push_back(name);
}

void ServerConfig::addPort(int port)
{
    this->_ports.push_back(port);
}

std::vector<std::string> ServerConfig::getServerNames(void) const
{
    return (this->_server_names);
}

std::vector<int> ServerConfig::getPorts(void) const
{
    return (this->_ports);
}

ServerConfig &ServerConfig::operator=(const ServerConfig &copy)
{
    std::cout << "[ServerConfig] copy assigment called" << std::endl;
    if (this != &copy)
    {
        this->_ports = copy._ports;
        this->_server_names = copy._server_names;
    }
    return (*this);
}