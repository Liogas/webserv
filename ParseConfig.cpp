/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 14:24:20 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"
# include "utils.hpp"

ParseConfig::ParseConfig(std::string path) : _path(path), _config(new ServerConfig())
{
    if (!isValidExtension(this->_path, "conf"))
        throw ParseConfig::ErrorExtension();
    this->_file.open(this->_path.c_str());
    if (!this->_file.is_open())
        throw ParseConfig::ErrorFile();
    this->_blocRoute = false;
    this->_blocServer = false;
    std::cout << "[ParseConfig] created with " << this->_path << std::endl;
}

void ParseConfig::startParsing(void)
{
    std::vector<std::string> contentFile = readFile(this->_file);
    for (size_t i = 0; i < contentFile.size(); i++)
    {
        std::cout << "Travaille sur la ligne : " << contentFile[i] << std::endl;
        std::vector<std::string> args = splitString(contentFile[i], ' ');
        if (args.size() > 0 && args[0].at(0) != '#')
        {
            if (!this->interpretArgs(args))
                throw ParseConfig::ErrorFileContent();
        }
    }
    std::cout << "Config du server : " << std::endl;
    std::cout << "Port : " << this->_config->getPort() << std::endl;
    std::cout << "Server_name : " << this->_config->getServerName() << std::endl;
}

bool ParseConfig::interpretArgs(std::vector<std::string> args)
{
    if (this->_blocRoute)
        return (this->interpretOnRoute(args));
    else if (this->_blocServer)
        return (this->interpretOnServer(args));
    else
    {
        if (args.size() == 2 && args[0] == "server" && args[1] == "{")
        {
            std::cout << "bloc server detecte" << std::endl;
            this->_blocServer = true;
        }
        else
            return (false);
    }
    return (true);
}

bool ParseConfig::interpretOnRoute(std::vector<std::string> args)
{
    int sizeArgs = args.size();
    if (sizeArgs == 1 && args[0] == "}")
    {
        std::cout << "Fin du bloc de route" << std::endl;
        this->_blocRoute = false;
    }
    else
        std::cout << "Ligne de route pas encore prete" << std::endl;
    return (true);
}

bool ParseConfig::interpretListen(std::vector<std::string> args)
{
    if (args.size() > 2)
    {
        std::cerr << "Error : to many args for listen" << std::endl;
        return (false);
    }
    for (size_t i = 0; i < args[1].size(); i++)
    {
    if (!isdigit(args[1].at(i)))
    {
            std::cerr << "Error: listen args on config file" << std::endl;
            return (false);
        }
    }
    int num;
    std::istringstream(args[1]) >> num;
    this->_config->setPort(num);
    return (true);
}

bool ParseConfig::interpretServerNames(std::vector<std::string> args)
{
    if (args.size() > 2)
    {
        std::cerr << "Error : to many args for server_names" << std::endl;
        return (false);
    }
    this->_config->setServerName(args[1]);
    return (true);
}

bool ParseConfig::interpretOnServer(std::vector<std::string> args)
{
    int sizeArgs = args.size();
    if (sizeArgs >= 2 && args[0] == "listen")
    {
        std::cout << "parametre listen detecte" << std::endl;
        return (this->interpretListen(args));
    }
    else if (sizeArgs >= 2 && args[0] == "server_names")
    {
        std::cout << "parametre server_names detecte" << std::endl;
        return (this->interpretServerNames(args));
    }
    else if (sizeArgs == 3 && args[0] == "location" && args[2] == "{")
    {
        std::cout << "Debut d'une nouvelle route" << std::endl;
        this->_blocRoute = true;
    }
    else if (sizeArgs == 1 && args[0] == "}")
    {
        this->_blocServer = false;
        std::cout << "Fin du bloc serveur" << std::endl;
    }
    else
        return (false);
    return (true);
}

ParseConfig::~ParseConfig(void)
{
    if (this->_file.is_open())
        this->_file.close();
    if (this->_config)
    {
        delete this->_config;
        this->_config = NULL;
    }
    std::cout << "[ParseConfig] destructor called" << std::endl;
}

ParseConfig &ParseConfig::operator=(const ParseConfig &copy)
{
    if (this != &copy)
    {
        this->_blocRoute = copy.getBlocRoute();
        this->_blocServer = copy.getBlocServer();
    }
    return (*this);
}

ServerConfig ParseConfig::getConfig(void) const
{
    return (*this->_config);
}

bool ParseConfig::getBlocRoute(void) const
{
    return (this->_blocRoute);
}

bool ParseConfig::getBlocServer(void) const
{
    return (this->_blocServer);
}

std::string ParseConfig::getPath(void) const
{
    return (this->_path);
}

// ServerConfig ParseConfig::getConfig(void) const
// {
//     return (this->_config);
// }

// std::vector<ServerConfig> ParseConfig::getConfigs(void) const
// {
//     return (this->_configs);
// }

const char *ParseConfig::ErrorExtension::what() const throw()
{
    return ("[!ParseConfig!] extension not valid");
}

const char *ParseConfig::ErrorFile::what() const throw()
{
    return ("[!ParseConfig!] file not found");
}

const char *ParseConfig::ErrorFileContent::what() const throw()
{
    return ("[!ParseConfig!] bad value on file");
}
