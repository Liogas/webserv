/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:28:54 by glions            #+#    #+#             */
/*   Updated: 2025/03/31 11:14:09 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

# define MAX_CLIENTS 5
# define BUFFER_SIZE 64000

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
        if (it != this->_servers.end())
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
        std::cout << "De nouveau ici" << std::endl;
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
    size_t bytesRead = recv(clientFd, buffer, sizeof(buffer), 0);
    std::cout << "BUFFER DE RECV : "<< std::endl;
    std::cout << buffer << std::endl;
    if (bytesRead == 0)
    {
        serv->eraseClient(it->first);
        client->disconnect();
        delete client;
        return (true);
    }
    // ERROR RECV
    else if (bytesRead < 0)
    {
        perror("recv");
        std::cout << errno << std::endl;
        return (false);
    }
    if (client->addBuffer(buffer, bytesRead))
    {
        Request req(client->getBuffer(), serv, client);
        req.handleRequest();
    }
    return (true);
}

// bool Webserv::handleClient(int clientFd)
// {
//     Server *serv = this->whereIsClient(clientFd);
//     if (!serv)
//     {
//         std::cerr << "client " << clientFd
//             << " unknown" << std::endl;
//         return (false);
//     }
//     std::map<int, Client *> clients = serv->getClients();
//     std::map<int, Client *>::iterator it = clients.find(clientFd);
//     if (it == clients.end())
//     {
//         std::cerr << "ERROR REALLY STRANGE" << std::endl;
//         return (false);
//     }
//     Client *client = it->second;

//     std::string bufferAll;
//     std::string header;
//     std::string body;
//     // ssize_t bytesAll = 0;
//     size_t contentLength = 0;
//     size_t bytes = 0;
//     char buffer[BUFFER_SIZE] = {0};
//     ssize_t bytesRead = 0;
//     while (1)
//     {
//         buffer[0] = 0;
//         bytes_read = recv(clientFd, buffer, sizeof(buffer) -1, MSG_DONTWAIT);
//         bytes+= bytes_read;
//         std::cout << "total read = " << bytes << std::endl; 
//         // CLIENT DISCONNECT
//         if (bytes_read == 0)
//         {
//             serv->eraseClient(it->first);
//             client->disconnect();
//             delete client;
//             return (true);
//         }
//         // ERROR RECV
//         else if (bytes_read < 0)
//         {
//             perror("recv");
//             std::cout << errno << std::endl;
//             return (false);
//         }
//         if (contentLength == 0) //Traiter les en ttes
//         {
//             header += buffer;
//             size_t posEmpty = header.find("\r\n\r\n");
//             if (posEmpty != std::string::npos)
//             {
//                 bytes =  bytes - posEmpty;
//                 size_t pos = header.find("Content-Length:");
//                 if (pos != std::string::npos) //POST
//                 {
//                     pos += 15;  // Passer "Content-Length: "
//                     size_t end = header.find("\r\n", pos); // Trouver la fin de la valeur de Content-Length
//                     if (end != std::string::npos)
//                     {
                        
//                         std::cout << "pos empty" << posEmpty << std::endl;
                       
                        
//                     }
//                     // Extraire le body à partir de la fin des en-têtes
//                     body = header.substr(posEmpty + 4); // +4 pour sauter "\r\n\r\n"
//                     header.erase(0, posEmpty + 4); // Enlever le header de l'objet header
//                     bytes -= header.size();
//                     std::cout << "header size -> " << header.size() << std::endl;
//                     std::cout << "---- HEADER ---- " << std::endl;
//                     std::cout << header << std::endl;
//                     std::cout << "---END HEADER---"  << std::endl;
//                     std::cout << "Contenu de mon body a ce moment -> " << body << " -> " << body.size() << std::endl;
//                     if (bytes >= contentLength)
//                         break ;
//                 }
//                 else  // METHOD GET
//                     break ;
//             }
//         }
//         else
//         {
//             body += buffer;
            
//             std::cout << "contentLength = " << contentLength << "       body size= " << body.size() << "     ";
//             if (bytes >= contentLength)
//             {
                
//                 break ;
//             }
//         }
//     }
//     std::cout << "tooooooooz" << std::endl;
//     bytes_read = recv(clientFd, buffer, sizeof(buffer) -1, MSG_DONTWAIT);

//     std::cout << buffer<< std::endl;
//     bufferAll = header + body;
//     bufferAll[bufferAll.size()] = '\0';
//     // CLIENT PART
//     Request req(bufferAll, serv, client);
//     req.handleRequest();
//     return (true);
// }


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