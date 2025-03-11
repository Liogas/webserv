/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/11 15:35:46 by glions           ###   ########.fr       */
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
    this->_blocRoot = false;
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
            {
                throw ParseConfig::ErrorFileContent();
            }
        }
    }
}

bool ParseConfig::interpretArgs(std::vector<std::string> args)
{
    if (this->_blocRoot)
        return (this->interpretOnRoot(args));
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

bool ParseConfig::interpretOnRoot(std::vector<std::string> args)
{
    int sizeArgs = args.size();
    if (sizeArgs == 1 && args[0] == "}")
    {
        std::cout << "Fin du bloc de route" << std::endl;
        this->_blocRoot = false;
    }
    else
        std::cout << "Ligne de route pas encore prete" << std::endl;
    return (true);
}

bool ParseConfig::interpretListen(std::vector<std::string> args)
{
    for (std::vector<std::string>::iterator it = args.begin() + 1; it != args.end(); ++it)
    {
        std::cout << "ARGS : " << *it << std::endl;
    }
    return (true);
}

bool ParseConfig::interpretOnServer(std::vector<std::string> args)
{
    int sizeArgs = args.size();
    if (sizeArgs >= 2 && args[0] == "listen")
    {
        std::cout << "parametre listen detecte" << std::endl;
        this->interpretListen(args);
    }
    else if (sizeArgs >= 2 && args[0] == "server_names")
    {
        std::cout << "parametres server_names detecte" << std::endl;
    }
    else if (sizeArgs == 3 && args[0] == "location" && args[2] == "{")
    {
        std::cout << "Debut d'une nouvelle route" << std::endl;
        this->_blocRoot = true;
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
        this->_blocRoot = copy.getBlocRoot();
        this->_blocServer = copy.getBlocServer();
    }
    return (*this);
}

bool ParseConfig::getBlocRoot(void) const
{
    return (this->_blocRoot);
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
