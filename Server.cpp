#include "Server.hpp"

Server::Server(void) :
    _fd(-1),
    _running(false),
    _config(NULL)
{}

Server::Server(ServerConfig *config) :
    _fd(-1),
    _running(false),
    _config(config)
{}

Server::~Server(void)
{
    if (this->_clients.size() > 0)
    {
        for (std::map<int, Client*>::iterator it = this->_clients.begin();
            it != this->_clients.end(); ++it)
        {
            it->second->disconnect();
            delete it->second;
        }
        this->_clients.erase(this->_clients.begin(), this->_clients.end());
    }
    if (this->_config)
    {
        delete this->_config;
        this->_config = NULL;
    }
    if (this->_fd != -1)
        close(this->_fd);
    std::cout << "[Server " << this->_fd << "] destroyed" << std::endl;
}

/*
    Methode qui genere le fd du serveur avec socket. Fcntl est utile pour le rendre
    non bloquant.
    setsockopt c'est pour pouvoir relancer un serveur sur le meme port je crois, c'est
    utile pour ne pas avoir a attendre avant de relancer le programme. Sinon on avait
    une erreur "bind: adress already use" ou truc dans le genre. Faudra peut etre la
    retirer a la fin.
*/
bool Server::init(void)
{
    this->_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    fcntl(this->_fd, F_SETFL, O_NONBLOCK);
    if (this->_fd == -1)
    {
        std::string error = "[!Server " + toString(this->getFd()) + "!] socket";
        perror(error.c_str());
        return (false);
    }
    int reuse = 1;
    setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    std::cout << "[Server " << toString(this->getFd()) <<
        "] socket initialized" << std::endl;
    return (true);
}

/*
    Methode qui s'occupe du bind et du listen pour le serveur.
    AF_INET -> On utilise le protocole IPv4
    INADDR_ANY -> On accepte toutes les adresses IP
    htons() -> convertit notre port qui est un int en format reseau (big endian)
*/
bool Server::bindSocket(int maxClient)
{
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = INADDR_ANY;
    this->_address.sin_port = htons(this->_config->getPort());
    if (bind(this->_fd,
            reinterpret_cast<struct sockaddr*>(&this->_address),
            sizeof(this->_address)) == -1)
    {
        std::string error = "[!Server " + toString(this->getFd()) + "!] bind";
        perror(error.c_str());
        return (false);
    }
    if (listen(this->_fd, maxClient) == -1)
    {
        std::string error = "[!Server " + toString(this->getFd()) + "!] listen";
        perror(error.c_str());
        return (false);
    }
    std::cout << "[Server " << this->_fd << "] Listening on port "
        << ntohs(this->_address.sin_port) << std::endl;
    return (true);
}

/*
    Methode qui permet d'ajouter un client au serveur.
    Accept va gerener le socket de ce client (revoir les parametres, je sais pas si 
    c'est bien de mettre NULL)
    fcntl met le socket en non bloquant.
    Apres ces etapes on creer un objet Client que l'on ajoute a la map clients du
    serveur (avec pour cle le fd du client).
*/
bool Server::newClient(void)
{
    int client_fd = accept(this->_fd, NULL, NULL);
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    if (client_fd == -1)
    {
        perror("accept");
        return (false);
    }
    try
    {
        Client *client = new Client(client_fd, *this);
        this->_clients.insert(std::make_pair(client_fd, client));
    } catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (false);
    }
    std::cout << "[Server " << this->_fd << "] new client connected : " << client_fd << std::endl;
    return (true);
}

/*
    Methode qui retire un client de la map clients s'il est trouve.
*/
void Server::eraseClient(int clientFd)
{
    if (this->_clients.find(clientFd) != this->_clients.end())
        this->_clients.erase(clientFd);
    else
        std::cerr << "eraseClient : Client unknown" << std::endl;
}

void Server::print(void)
{
    std::cout << "FD : " << this->_fd << std::endl;
    std::cout << "Running : " << this->_running << std::endl;
    this->_config->print();
}

// SETTERS
void Server::setEpollFd(int fd)
{
    this->_epollFd = fd;
}

// GETTERS
int Server::getFd(void) const
{
    return (this->_fd);
}

int Server::getEpollFd(void) const
{
    return (this->_epollFd);
}

ServerConfig *Server::getConfig(void) const
{
    return (this->_config);
}

std::map<int, Client *> Server::getClients(void) const
{
    return (this->_clients);
}

// EXCEPTIONS
const char *Server::ErrorSocket::what() const throw()
{
    return ("Error creation socket on Server constructor");
}

const char *Server::ErrorBind::what() const throw()
{
    return ("Error on bindSocket method");
}
