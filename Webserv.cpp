/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:28:54 by glions            #+#    #+#             */
/*   Updated: 2025/03/20 13:33:33 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

# define MAX_CLIENTS 5
# define BUFFER_SIZE 1024

Webserv::Webserv() :
    _servers(),
    _epollFd(-1)
{
    std::cout << "[Webserv] created" << std::endl;
}

Webserv::Webserv(Webserv const & copy) :
    _servers(copy.getServers()),
    _epollFd(-1)
{
    std::cout << "[Webserv] copy constructor called" << std::endl;
}

Webserv::~Webserv(void)
{
    for (size_t i = 0; i < this->_servers.size(); i++)
        delete this->_servers[i];
    if (this->_epollFd != -1)
        close(this->_epollFd);
    std::cout << "[Webserv] destroyed" << std::endl;
}

bool Webserv::parsing(std::string file_path)
{
    try
    {
        ParseConfig parsing(file_path);
        if (!parsing.startParsing())
            return (false);
        std::vector<ServerConfig *> configs = parsing.getConfigs();
        for (size_t i = 0; i < configs.size(); i++)
        {
            ServerConfig *tmp = new ServerConfig(*configs[i]);
            this->_servers.push_back(new Server(tmp));
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (false);
    }
    return (true);
}

bool Webserv::bindServers(void)
{
    try
    {
        for (size_t i = 0; i < this->_servers.size(); i++)
            this->_servers[i]->bindSocket(MAX_CLIENTS);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (false);
    }
    return (true);
}

bool Webserv::ready(void)
{
    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
    {
        perror("epoll_create1");
        return (false);
    }
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        struct epoll_event event = {};
        this->_servers[i]->setEpollFd(this->_epollFd);
        event.events = EPOLLIN;
        event.data.fd = this->_servers[i]->getFd();
        if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD,
                this->_servers[i]->getFd(), &event) == -1)
        {
            perror("epoll_ctl");
            return (false);
        }
    }
    return (true);
}

bool Webserv::start(void)
{
    while (1)
    {
        struct epoll_event events[10];
        int nfds = epoll_wait(this->_epollFd, events, MAX_CLIENTS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            return (false);
        }
        for (int i = 0; i < nfds; i++)
        {
            int mode = 0;
            for (size_t j = 0; j < this->_servers.size(); j++)
            {
                if (this->_servers[j]->getFd() == events[i].data.fd)
                {
                    mode = 1;
                    std::cout << "SERVER EN ATTENTE D'AJOUT DU CLIENT : " << this->_servers[j]->getFd() << std::endl;
                    if (!this->_servers[j]->newClient())
                        break;
                }
            }
            if (mode == 0)
            {
                this->handleClient(events[i].data.fd);
            }
        }
    }
    return (true);
}

bool Webserv::handleClient(int clientFd)
{
    std::cout << "Entree dans handle client avec : " << clientFd << std::endl;
    Server *serv = this->whereIsClient(clientFd);
    if (!serv)
    {
        std::cout << "client " << clientFd
            << " unknown" << std::endl;
        return (false);
    }
    std::cout << "Le client se trouve sur le server " << serv->getFd() << std::endl;
    std::map<int, Client *> clients = serv->getClients();
    std::map<int, Client *>::iterator it = clients.find(clientFd);
    if (it == serv->getClients().end())
    {
        std::cerr << "ERREUR REALLY STRANGE" << std::endl;
        return (false);
    }
    Client *client = it->second;
    std::cout << "Je suis le client " << client->getFd() << std::endl;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = recv(clientFd, buffer, sizeof(buffer) -1, MSG_DONTWAIT);
    buffer[bytes_read] = '\0';
    if (bytes_read == 0)
    {
        client->disconnect();
        serv->getClients().erase(it);
        delete client;
        return (false);
    }
    else if (bytes_read < 0)
    {
        perror("recv");
        return (false);
    }
    std::cout << buffer << std::endl;
    return (true);
}

Server *Webserv::whereIsClient(int clientFd)
{
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        std::cout << "Je cherche dans le server -> " << this->_servers[i]->getFd() << std::endl;
        if (this->_servers[i]->getClients().find(clientFd) != this->_servers[i]->getClients().end())
        {

            return (this->_servers[i]);
        }
    }
    return (NULL);
}

void Webserv::printServers(void)
{
    std::cout << "SERVERS : " << std::endl;
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        this->_servers[i]->print();
        std::cout << std::endl;
    }
}

// GETTERS
std::vector<Server *> Webserv::getServers(void) const
{
    return (this->_servers);
}