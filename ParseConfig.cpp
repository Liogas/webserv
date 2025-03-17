/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/17 13:59:16 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"
# include "utils.hpp"

ParseConfig::ParseConfig(std::string path) : _path(path), _configs()
{
    if (!isValidExtension(this->_path, "conf"))
        throw ParseConfig::ErrorFileExtension();
    this->_file.open(this->_path.c_str());
    if (!this->_file.is_open())
        throw ParseConfig::ErrorFile();
    this->_blocRoute = false;
    this->_blocServer = false;
    std::cout << "[ParseConfig] created with " << this->_path << std::endl;
}

bool ParseConfig::startParsing(void)
{
    std::vector<std::string> contentFile = readFile(this->_file);
    if (contentFile.size() <= 0)
        throw ParseConfig::ErrorFileEmpty();
    for (size_t i = 0; i < contentFile.size(); i++)
    {
        std::vector<std::string> args = splitString(contentFile[i], ' ');
        cleanArgs(&args);
        if (args.size() == 2 && args[0] == "server" && args[1] == "{")
        {
            try
            {
                i++;
                this->parseServer(&i, contentFile);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what();
                return (false);
            }
        }
        else if (args.size() != 0)
        {
            std::cerr << "[!ParseConfig!] bad value on file" << std::endl;
            return (false);
        }
    }
    return (true);
}

void ParseConfig::parseServer(size_t *i, std::vector<std::string> lines)
{
    ServerConfig *conf = new ServerConfig();
    while (*i < lines.size())
    {
        std::vector<std::string> args = splitString(lines[*i], ' ');
        cleanArgs(&args);
        for (size_t i = 0; i < args.size(); i++)
        {
            std::cout << "args[" << i << "] = " << args[i] << std::endl;
        }
        // LISTEN
        if (args.size() >= 1 && args[0] == "listen")
        {
            try
            {
                std::cout << "Listen detected" << std::endl;
                conf->setPort(args);
            } catch (std::exception &e) {
                delete conf;
                std::cerr << "listen : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // SERVER_NAME
        else if (args.size() >= 1 && args[0] == "server_name")
        {
            try
            {
                std::cout << "Server name detected" << std::endl;
                conf->setServerName(args);
            } catch (std::exception &e) {
                delete conf;
                std::cerr << "server_name : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // ERROR_PAGE
        else if (args.size() >= 1 && args[0] == "error_page")
        {
            try
            {
                std::cout << "Error_page detected" << std::endl;
                conf->addErrorPage(args);
            } catch(const std::exception& e) {
                delete conf;
                std::cerr << "error_page : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // CLIENT_MAX_BODY
        else if (args.size() >= 1 && args[0] == "client_max_body")
        {
            try
            {
                std::cout << "Client max body detected" << std::endl;
                conf->setClientMaxBody(args);
            } catch (std::exception &e) {
                delete conf;
                std::cerr << "client_max_body : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // LOCATION -> NEW ROUTE
        else if (args.size() == 3 && args[0] == "location" && args[2] == "{")
        {
            try
            {
                std::cout << "Location detected" << std::endl;
                this->parseRoute(i, lines);
            } catch (std::exception &e) {
                delete conf;
                std::cerr << "location : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        else if (args.size() == 1 && args[0] == "}")
        {
            std::cout << "Config de mon server : " << std::endl;
            std::cout << "Port : " << conf->getPort() << std::endl;
            std::cout << "Server_name : " << conf->getServerName() << std::endl;
            std::cout << "Size max body : " << conf->getClientMaxBody() << std::endl;
            std::cout << "Error pages : " << std::endl;
            std::map<int, std::string> tmp = conf->getErrorPages();
            for (std::map<int, std::string>::iterator it = tmp.begin();
                it != tmp.end(); ++it)
            {
                std::cout << "Error : " << it->first << " -> " << it->second << std::endl; 
            }
            delete conf;
            return ;
        }
        else
        {
            delete conf;
            throw ParseConfig::ErrorFileContent();
        }
        (*i)++;
    }
    delete conf;
    throw ParseConfig::ErrorFileContent();
}

void ParseConfig::parseRoute(size_t *i, std::vector<std::string> lines)
{
    Route *route = new Route();
}

ParseConfig::~ParseConfig(void)
{
    if (this->_file.is_open())
        this->_file.close();
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

std::vector<ServerConfig *> ParseConfig::getConfigs(void) const
{
    return (this->_configs);
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

const char *ParseConfig::ErrorFileExtension::what() const throw()
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

const char *ParseConfig::ErrorFileEmpty::what() const throw()
{
    return ("[!ParseConfig!] file is empty");
}
