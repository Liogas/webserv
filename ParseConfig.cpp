/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/10 16:08:58 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"

ParseConfig::ParseConfig(std::string path) : _path(path)
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
        std::cout << "parsing de la ligne : " << contentFile[i] << std::endl;
        std::vector<std::string> args = splitString(contentFile[i], ' ');
        // std::cout << args.size() << " > 0 && " << args[0] << ".size() " << ">= 1 && " << args[0].at(0) << " != '#'" << std::endl;
        if (args.size() > 0 && args[0].size() >= 1 && args[0].at(0) != '#')
        {
            if (!this->interpretArgs(args))
                throw ParseConfig::ErrorFileContent();
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
    std::cout << "Entre dans interpretOnRoot" << std::endl;
    int sizeArgs = args.size();
    if (sizeArgs == 1 && args[0] == "{")
        this->_blocRoot = false;
    else
        std::cout << "Ligne de route pas encore prete" << std::endl;
    return (true);
}

bool ParseConfig::interpretOnServer(std::vector<std::string> args)
{
    std::cout << "Entre dans interpretOnServer" << std::endl;
    std::cout << "Args :" << std::endl;
    for (size_t i = 0; i < args.size(); i++)
    {
        std::cout << args[i] << std::endl;
    }
    int sizeArgs = args.size();
    if (sizeArgs >= 2 && args[0] == "listen")
    {
        std::cout << "parametre listen detecte" << std::endl;
    }
    else if (sizeArgs >= 2 && args[0] == "server_names")
    {
        std::cout << "parametres server_names detecte" << std::endl;
    }
    else if (sizeArgs == 3 && args[0] == "location" && args[3] == "{")
    {
        this->_blocRoot = true;
    }
    else if (sizeArgs == 1 && args[0] == "{")
        this->_blocServer = false;
    else
        return (false);
    return (true);
}

ParseConfig::~ParseConfig(void)
{
    if (this->_file.is_open())
        this->_file.close();
    std::cout << "[ParseConfig] destructor called" << std::endl;
}

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
