/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:28:54 by glions            #+#    #+#             */
/*   Updated: 2025/03/21 18:19:20 by glions           ###   ########.fr       */
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
    return (true);
}


/*
    Methode qui s'occupe du bind et du listen pour les differents serveurs crees.
*/
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

/*
    Methode qui creer le gestionnaire d'evenements (epoll). Elle ajoute dedans les
    differents serveurs lances.

    ! Doit etre lance apres bindServers() !
*/
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
    std::cout << "Mes serveurs : " << std::endl;
    this->printServers();
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
    // BUFFER READED
    std::vector<std::string> lines = splitString(buffer, '\n');
    std::vector<std::string> args = splitString(lines[0], ' ');
    std::cout << "ARGS : " << std::endl;
    for (size_t i = 0; i < args.size(); i++)
        std::cout << args[i] << std::endl;
    // std::cout << buffer << std::endl;
    if (isValidExtension(args[1], "html"))
        this->sendFile(clientFd, args[1]);
    else
        this->listDir(clientFd, args[1]); // Juste un test
    // FONCTION_LOLO(buffer, serv, client);
    return (true);
}

bool Webserv::listDir(int clientFd, std::string path)
{
    std::string root = "test";
    DIR *dir;
    struct dirent *entry;
    std::vector<struct dirent *> entrys;
    std::string fullpath = root + path;
    if (fullpath.find(root) != 0)
    {
        std::cerr << "SECURITY ALERT" << std::endl;
        return (false);
    }
    dir = opendir(fullpath.c_str());
    if (!dir)
    {
        std::cout << "ERROR open directory" << std::endl;
        return (false);
    }
    std::string body = "<html><body><h1>Contents of " + path + " :</h1><ul>";
    while ((entry = readdir(dir)) != NULL)
    {
        std::string link = (path == "/" ? "" : path) + "/" + entry->d_name;
        std::string tmp = "<li><a href=\"" + link + "\">" + entry->d_name + "</a></li>";
        std::cout << "Ma balise li contient -> " << tmp << std::endl;
        body += tmp;
    }
    body += "</ul></body></html>";
    closedir(dir);
    std::ostringstream oss;
    oss << body.size();
    std::string response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
    response += body;
    send(clientFd, response.c_str(), response.size(), 0);
    return (true);
}

bool Webserv::sendFile(int clientFd, std::string path)
{
    std::string root = "test";
    std::string fullpath = root + path;
    std::cout << fullpath << std::endl;
    std::ifstream file(fullpath.c_str());
    if (!file.is_open())
    {
        std::cerr << "ERROR OPEN FILE" << std::endl;
        return (false);
    }
    std::string line;
    std::string response_body;
    std::ostringstream oss;
    std::string response;
    while (std::getline(file, line))
        response_body += line;
    file.close();
    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: keep-alive\r\n";  // Ne ferme pas la connexion
    oss << response_body.size();
    response += "Content-Length: " + oss.str() + "\r\n";
    response += "\r\n";
    response += response_body;
    send(clientFd, response.c_str(), response.size(), 0);
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