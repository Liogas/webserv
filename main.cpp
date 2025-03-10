/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:51:15 by glions            #+#    #+#             */
/*   Updated: 2025/03/10 15:51:52 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "Try : ./webserv [config.conf]" << std::endl;
        return (1);
    }
    ParseConfig *config = NULL;
    try
    {
        config = new ParseConfig(av[1]);
        config->startParsing();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        delete config;
        return (1);
    }
    delete config;
    return (0);
}