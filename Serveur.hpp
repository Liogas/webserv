#ifndef SERVEUR_HPP
# define SERVEUR_HPP

# include <iostream>
# include <map>
# include <exception>
# include <sys/epoll.h>
# include <sys/socket.h>
# include <netinet/in.h>
#include <unistd.h>
# include <cstdio>
#include <sstream>

/*
    Idees :
        - Creer un constructeur attendant un ficher de config
*/

class Serveur
{
    public:
        Serveur(void);
        ~Serveur();
        void bindSocket(sa_family_t family, in_addr_t s_addr, int port);
        void ready(void);
        void start(void);
        void acceptClient(void);
        void handleClient(int client_fd);
    private:
        int _fd;
        int _epollFd;
        bool _running;
        struct sockaddr_in _address;

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