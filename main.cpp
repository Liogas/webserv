/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:51:15 by glions            #+#    #+#             */
/*   Updated: 2025/03/20 15:05:53 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include <csignal>
#include <cstdlib>

Webserv *webserv = NULL;

// GESTION DU CTRL+C
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
    signal(SIGINT, signalHandler); // DETECTE LE CTRL+C
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
    delete webserv;
    return (0);
}
