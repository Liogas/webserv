/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:51:15 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 14:34:49 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"
#include "Server.hpp"
#include <csignal>
#include <cstdlib>

Server *server = NULL;
ParseConfig *parseConfig = NULL;

void signalHandler(int signum)
{
    if (server)
    {
        delete server;
        server = NULL;
    }
    if (parseConfig)
    {
        delete parseConfig;
        parseConfig = NULL;
    }
    exit(signum);
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "Try : ./webserv [config.conf]" << std::endl;
        return (1);
    }
    signal(SIGINT, signalHandler);
    try
    {
        parseConfig = new ParseConfig(av[1]);
        parseConfig->startParsing();
        server = new Server();
        server->bindSocket(AF_INET, INADDR_ANY, parseConfig->getConfig().getPort());
        server->ready();
        server->start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        delete parseConfig;
        parseConfig = NULL;
        if (server)
            delete server;
        server = NULL;
        return (1);
    }
    delete parseConfig;
    delete server;
    return (0);
}
