#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <unistd.h>
# include <iostream>

class Request
{
    public:
        Request(ssize_t bytes, char *buffer);
        ~Request();
        ssize_t getBytesRead(void);
        std::string getBuffer(void);
    private:
        ssize_t _bytes_read;
        std::string _buffer;
};

#endif