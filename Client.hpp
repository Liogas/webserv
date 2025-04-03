#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"

# include <errno.h>
# include <string.h>

class Server;

class Client
{
    public:
        Client(int fd, Server &serv);
        ~Client();
        void disconnect();
        void newRequest(std::string buffer);
        void updateRequest(std::string buffer, ssize_t bytes);
        void deleteRequest(void);
        bool addBuffer(std::string, ssize_t bytesRead);
        void resetBuffer(void);
        int parseRequest(void);
        bool requestReady(void);
        // GETTERS
        int getFd(void) const;
        std::string getBuffer(void) const;
        Request *getCurrReq(void) const;
    private:
        Server &_serv;
        int _fd;
        std::string _buffer;
        ssize_t _bytes;
        int _contentLength;
        Request *_currReq;
    class ErrorClient : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif