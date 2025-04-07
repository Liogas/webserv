#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "ParseRequest.hpp"

# include <errno.h>
# include <string.h>


class ParseRequest;
class Server;

class Client
{
    public:
        Client(int fd, Server &serv);
        ~Client();
        void disconnect();
        void newParseRequest(std::string buffer, ssize_t bytes);
        void updateParseRequest(std::string buffer, ssize_t bytes);
        void deleteParseRequest(void);
        int requestReady(void);
        int  parseRequest(void);
        // GETTERS
        int getFd(void) const;
        std::string getBuffer(void) const;
        ParseRequest *getParseReq(void) const;
    private:
        Server &_serv;
        int _fd;
        std::string _buffer;
        ssize_t _bytes;
        int _contentLength;
        ParseRequest *_parseReq;
    class ErrorClient : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif