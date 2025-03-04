#include "Serveur.hpp"

/*
    strace -e trace=socket ./webserv -> Trace les appels systemes (socket)
*/

int main(void)
{
    try
    {
        Serveur serv;
        serv.bindSocket(AF_INET, INADDR_ANY, 8080);
        serv.ready();
        serv.start();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

