#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/epoll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>

# include <map>
# include <sstream>
# include <cstdio>
# include <fstream>

# include "Client.hpp"
# include "ServerConfig.hpp"

class Client;

class Server
{
    public:
        Server(void);
        Server(ServerConfig *config);
        ~Server();
        bool init(void);
        void print(void);
        bool bindSocket(int maxClient);
        bool newClient(void);
        void eraseClient(int clientFd);
        // GETTERS
        int getFd(void) const;
        std::map<int, Client *> getClients(void) const;
        int getEpollFd(void) const;
        ServerConfig *getConfig(void) const;
        // SETTERS
        void setEpollFd(int fd);
    private:
        int _fd;
        int _epollFd;
        bool _running;
        struct sockaddr_in _address;
        ServerConfig *_config;
        std::map<int, Client*> _clients;

    class ErrorSocket : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorBind : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif