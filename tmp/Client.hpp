#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

class Client
{
    public:
        Client(void);
        Client(int fd);
        ~Client();
        int getFd(void);
    private:
        int _fd;
};

#endif