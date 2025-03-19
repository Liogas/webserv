/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/19 11:31:47 by glions           ###   ########.fr       */
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
                ServerConfig *tmp = this->parseServer(&i, contentFile);
                this->_configs.push_back(tmp);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                return (false);
            }
        }
        else if (args.size() != 0)
        {
            std::cerr << "bad value on file" << std::endl;
            return (false);
        }
    }
    return (true);
}

ServerConfig *ParseConfig::parseServer(size_t *i, std::vector<std::string> lines)
{
    ServerConfig *conf = new ServerConfig();
    while (*i < lines.size())
    {
        std::vector<std::string> args = splitString(lines[*i], ' ');
        cleanArgs(&args);
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
                Route *tmp = this->parseRoute(i, lines);
                conf->addRoute(tmp);
            } catch (std::exception &e) {
                delete conf;
                std::cerr << "location : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // END OF BLOC SERVER
        else if (args.size() == 1 && args[0] == "}")
            return (conf);
        else if (args.size() != 0)
        {
            delete conf;
            throw ParseConfig::ErrorFileContent();
        }
        (*i)++;
    }
    delete conf;
    throw ParseConfig::ErrorFileContent();
}

Route *ParseConfig::parseRoute(size_t *i, std::vector<std::string> lines)
{
    std::vector<std::string> args = splitString(lines[*i], ' ');
    cleanArgs(&args);
    Route *route = new Route(args[1]);
    while (++(*i) < lines.size())
    {
        args = splitString(lines[*i], ' ');
        cleanArgs(&args);
        // METHODS
        if (args.size() >= 1 && args[0] == "methods")
        {
            try
            {
                route->addMethods(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "methods : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // RETURN -> REDIRECTION
        else if (args.size() >= 1 && args[0] == "return")
        {
            try
            {
                route->setRedir(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "return : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // ROOT
        else if (args.size() >= 1 && args[0] == "root")
        {
            try
            {
                route->setRoot(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "root : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // AUTOINDEX
        else if (args.size() >= 1 && args[0] == "autoindex")
        {
            try
            {
                route->setAutoIndex(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "autoindex : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // INDEX
        else if (args.size() >= 1 && args[0] == "index")
        {
            try
            {
                route->setIndex(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "index : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        // END OF BLOC ROUTE
        else if (args.size() == 1 && args[0] == "}")
        {
            if (route->getAutoIndex() == 2)
                route->setAutoIndex(0);
            return (route);
        }
        else if (args.size() != 0)
        {
            delete route;
            throw ParseConfig::ErrorFileContent();
        }
    }
    delete route;
    throw ParseConfig::ErrorFileContent();
}

ParseConfig::~ParseConfig(void)
{
    if (this->_file.is_open())
        this->_file.close();
    for (size_t i = 0; i < this->_configs.size(); i++)
    {
        if (this->_configs[i])
            delete this->_configs[i];
    }
    std::cout << "[ParseConfig] destructor called" << std::endl;
}

ParseConfig &ParseConfig::operator=(const ParseConfig &copy)
{
    if (this != &copy)
    {
    }
    return (*this);
}

ServerConfig *ParseConfig::extractConfig(size_t pos)
{
    ServerConfig *tmp = this->_configs[pos];
    this->_configs[pos] = NULL;
    return (tmp);
}

std::vector<ServerConfig *> ParseConfig::getConfigs(void) const
{
    return (this->_configs);
}

std::string ParseConfig::getPath(void) const
{
    return (this->_path);
}

const char *ParseConfig::ErrorFileExtension::what() const throw()
{
    return ("extension not valid");
}

const char *ParseConfig::ErrorFile::what() const throw()
{
    return ("file not found");
}

const char *ParseConfig::ErrorFileContent::what() const throw()
{
    return ("bad value on file");
}

const char *ParseConfig::ErrorFileEmpty::what() const throw()
{
    return ("file is empty");
}
