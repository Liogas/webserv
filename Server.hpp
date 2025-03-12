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
        ~Server();
        void bindSocket(sa_family_t family, in_addr_t s_addr, int port);
        void ready(void);
        void start(void);
        void acceptClient(void);
        void handleClient(int client_fd);
        int getEpollFd(void);
    private:
        int _fd;
        int _epollFd;
        bool _running;
        struct sockaddr_in _address;
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