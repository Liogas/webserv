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
        bool addBuffer(std::string, ssize_t bytesRead);
        // GETTERS
        int getFd(void) const;
        std::string getBuffer(void) const;
    private:
        Server &_serv;
        int _fd;
        std::string _buffer;
        ssize_t _bytes;
        int _contentLength;
    class ErrorClient : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif