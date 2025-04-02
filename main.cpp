/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:51:15 by glions            #+#    #+#             */
/*   Updated: 2025/04/02 13:56:18 by glions           ###   ########.fr       */
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
    std::string file;   
    if (ac != 2)
        file = "./configs/test.conf";
    else
        file = av[1];
    signal(SIGINT, signalHandler); // DETECTE LE CTRL+C
    webserv = new Webserv();
    if (!webserv->parsing(file) ||
            !webserv->initServers() ||
            !webserv->ready() ||
            !webserv->start()
        )
    {
        delete webserv;
        webserv = NULL;
        return (1);
    }
    delete webserv;
    return (0);
}
