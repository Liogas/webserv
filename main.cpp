#include "Server.hpp"
#include <csignal>
#include <cstdlib>

/*
    strace -e trace=socket ./webserv -> Trace les appels systemes (socket)
*/

Server *serv = NULL;

void signalHandler(int signum)
{
    if (serv)
    {
        delete serv;
        serv = NULL;
    }
    exit(signum);
}

int main(void)
{
    signal(SIGINT, signalHandler);
    try
    {
        serv = new Server();
        serv->bindSocket(AF_INET, INADDR_ANY, 8080);
        serv->ready();
        serv->start();
        delete serv;
        serv = NULL;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        if (serv)
        {
            delete serv;
            serv = NULL;
        }
    }
}

