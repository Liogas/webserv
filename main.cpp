/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:51:15 by glions            #+#    #+#             */
/*   Updated: 2025/03/19 15:50:32 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <csignal>
#include <cstdlib>

Webserv *webserv = NULL;

void signalHandler(int signum)
{
    if (webserv)
        delete webserv;
    exit(signum);
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "Try : ./webserv [file.conf]" << std::endl;
        return (1);
    }
    signal(SIGINT, signalHandler);
    webserv = new Webserv();
    if (!webserv->parsing(av[1]) ||
        !webserv->bindServers() ||
        !webserv->ready() ||
        !webserv->start())
    {
        delete webserv;
        webserv = NULL;
        return (1);
    }
    
    // webserv->printServers();
    delete webserv;
    return (0);
}

// int main(int ac, char **av)
// {
//     if (ac != 2)
//     {
//         std::cerr << "Try : ./webserv [config.conf]" << std::endl;
//         return (1);
//     }
//     signal(SIGINT, signalHandler);
//     try
//     {
//         parseConfig = new ParseConfig(av[1]);
//         if (!parseConfig->startParsing())
//         {
//             delete parseConfig;
//             parseConfig = NULL;
//             return (1);
//         }
//         // std::cout << "Routes : " << std::endl;
//         // std::map<std::string, Route *> routes = parseConfig->getConfig()->getRoutes();
//         // for (std::map<std::string,Route *>::iterator it = routes.begin();
//         //     it != routes.end();
//         //     ++it)
//         // {
//         //     std::cout << it->first << std::endl;
//         // }
//         // server = new Server();
//         // server->bindSocket(AF_INET, INADDR_ANY, parseConfig->getConfig()->getPort());
//         // server->ready();
//         // server->start();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << std::endl;
//         delete parseConfig;
//         parseConfig = NULL;
//         if (server)
//             delete server;
//         server = NULL;
//         return (1);
//     }
//     delete parseConfig;
//     delete server;
//     return (0);
// }
