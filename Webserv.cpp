/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:28:54 by glions            #+#    #+#             */
/*   Updated: 2025/03/25 11:34:31 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

# define MAX_CLIENTS 5
# define BUFFER_SIZE 1024

Webserv::Webserv() :
    _servers(),
    _epollFd(-1)
{
}

Webserv::~Webserv(void)
{
    for (size_t i = 0; i < this->_servers.size(); i++)
        delete this->_servers[i];
    if (this->_epollFd != -1)
        close(this->_epollFd);
}

/*
    Methode qui gert le parsing en creant un objet de la classe ParseConfig.
    Une fois le parsing fait s'il y a pas eu d'erreurs, les serveurs sont creer
    avec les objets ServerConfig presents dans l'objet ParseConfig puis ajouter
    dans le vector _servers.
*/
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
    this->printServers();
    return (true);
}

bool Webserv::initServers(void)
{
    int error = 0;
    int nbServ = this->_servers.size();
    int i = 0;
    for (std::vector<Server *>::iterator it = this->_servers.begin();
        it != this->_servers.end();)
    {
        i++;
        if ((*it)->getFd() == -1 && (
                !(*it)->init() ||
                !(*it)->bindSocket(MAX_CLIENTS)))
        {
            delete *it;
            error++;
            it = this->_servers.erase(it);
            continue ;
        }
        it++;
    }
    return (error == nbServ) ? false : true;
}

/*
    Methode qui creer le gestionnaire d'evenements (epoll). Elle ajoute dedans les
    differents serveurs lances.

    ! Doit etre lance apres bindServers() !
*/
bool Webserv::ready(void)
{
    int nbError = 0;
    int nbServer = this->_servers.size();
    this->_epollFd = epoll_create1(0);
    if (this->_epollFd == -1)
    {
        perror("epoll_create1");
        return (false);
    }
    for (std::vector<Server *>::iterator it = this->_servers.begin();
            it != this->_servers.end();)
    {
        struct epoll_event event = {};
        (*it)->setEpollFd(this->_epollFd);
        event.events = EPOLLIN;
        event.data.fd = (*it)->getFd();
        if (epoll_ctl(this->_epollFd, EPOLL_CTL_ADD,
                (*it)->getFd(), &event) == -1)
        {
            std::string error = "[!Server "
                + toString((*it)->getFd()) + "!] epoll_ctl";
            perror(error.c_str());
            delete *it;
            nbError++;
            it = this->_servers.erase(it);
            continue ;
        }
        std::cout << "[Server " << (*it)->getFd() << "] started" << std::endl;
        it++;
    }
    return (nbError == nbServer) ? true : true;
}

/*
    Methode qui fait tourner avec une boucle infini les differents serveurs.
    C'est ici qu'on detecte lorsque le serveur recoit un evenement grace a epoll_wait.
    Apres on recupere events[i].data.fd qui permet de savoir qui a declencher l'evenement.
    
    - Si FD correspond a un serveur ca veut dire que c'est un nouveau client qui veut
    se connecter au serveur. Dans ce cas on appel newClient dans lequel on utilise la
    fonction accept()
    - Sinon c'est un client deja present sur un serveur et on appelle la method
    handleClient.
*/
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
                    if (!this->_servers[j]->newClient())
                        break;
                }
            }
            if (mode == 0)
                this->handleClient(events[i].data.fd);
        }
    }
    return (true);
}

/*
    Methode utile pour gerer un evenement provenant d'un client.
    Dans un premier on cherche le serveur sur lequel il se trouve.
    Ensuite on recupere la requete envoye par le client avec recv.
    - Si bytes_read = 0 le client s'est deconnecte du serveur
    - Si < 0 erreur lors de l'appel de recv
    - Sinon recv s'est bien passe et la requete stockee dans le buffer.
    Ensuite il faudra appeler la fonction de Loreen avec le client, le buffer et
    le serveur en parametre.
*/
bool Webserv::handleClient(int clientFd)
{
    Server *serv = this->whereIsClient(clientFd);
    if (!serv)
    {
        std::cerr << "client " << clientFd
            << " unknown" << std::endl;
        return (false);
    }
    std::map<int, Client *> clients = serv->getClients();
    std::map<int, Client *>::iterator it = clients.find(clientFd);
    if (it == clients.end())
    {
        std::cerr << "ERROR REALLY STRANGE" << std::endl;
        return (false);
    }
    Client *client = it->second;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = recv(clientFd, buffer, sizeof(buffer) -1, MSG_DONTWAIT);
    buffer[bytes_read] = '\0';
    // CLIENT DISCONNECT
    if (bytes_read == 0)
    {
        serv->eraseClient(it->first);
        client->disconnect();
        delete client;
        return (true);
    }
    // ERROR RECV
    else if (bytes_read < 0)
    {
        perror("recv");
        return (false);
    }
    // CLIENT PART
    Request req(buffer, serv, client);
    req.handleRequest();
    return (true);
}


/*
    Methode qui prend le fd du client recherche en parametre.
    Parcourt les serveurs puis cherche dans chacun d'entre eux si le client est
    present puis retourne le serveur si c'est le cas.
*/
Server *Webserv::whereIsClient(int clientFd)
{
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        std::map<int, Client *> clients = this->_servers[i]->getClients();
        if (clients.find(clientFd) != clients.end())
            return (this->_servers[i]);
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