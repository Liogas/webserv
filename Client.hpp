#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"

class Server;

class Client
{
    public:
        Client(int fd, Server &serv);
        ~Client();
        void disconnect();
        // GETTERS
        int getFd(void) const;
    private:
        Server &_serv;
        int _fd;
    class ErrorClient : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif