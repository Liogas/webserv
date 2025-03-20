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

# include "Request.hpp"
# include "Client.hpp"
# include "ServerConfig.hpp"

/*
    Idees :
        - Creer un constructeur attendant un ficher de config
*/

class Client;
class Request;

class Server
{
    public:
        Server(void);
        Server(ServerConfig *config);
        ~Server();
        bool init(void);
        void print(void);
        void bindSocket(int maxClient);
        int getFd(void) const;
        bool newClient(void);
        std::map<int, Client *> getClients(void) const;
        int getEpollFd(void) const;
        void setEpollFd(int fd);
        void eraseClient(int clientFd);
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
    class ErrorReady : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorStart : public std::exception
    {
        public :
            virtual const char *what() const throw();
    };
    class ErrorClient : public std::exception
    {
        public :
            virtual const char *what() const throw();
    };
};

#endif