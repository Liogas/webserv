#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"

class Server;

class Client
{
    public:
        Client(int fd, Server &serv);
        ~Client();
        Request readRequest(void);
        void sendResponse(std::string response);
        void disconnect();
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