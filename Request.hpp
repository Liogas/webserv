#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <unistd.h>
# include <exception>
# include "utils.hpp"

class Request
{
    public:
        Request(ssize_t bytes, char *buffer);
        ~Request();
        ssize_t getBytesRead(void);
        std::string getBuffer(void);
        Method getRequestMethod(void);
        std::string getRequestPath(void);
        std::string getRequestVersion(void);
        void parseRequestLine(void);
        void verifRequestLine(void);
    private:
        ssize_t _bytes_read;
        std::string _buffer;
        Method _request_method;
        std::string _request_path;
        std::string _request_version;
    
    class ErrorVerifRequestLine : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorParseRequestLine : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif