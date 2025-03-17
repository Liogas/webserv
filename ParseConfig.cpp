/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/17 16:22:22 by glions           ###   ########.fr       */
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
                ServerConfig *tmp = this->parseServer(&i, contentFile);
                this->_configs.push_back(tmp);
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

ServerConfig *ParseConfig::parseServer(size_t *i, std::vector<std::string> lines)
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
                Route *tmp = this->parseRoute(i, lines);
                conf->addRoute(tmp);
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
            return (conf);
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
                std::cout << "Method detected" << std::endl;
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
                std::cout << "return detected" << std::endl;
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
                std::cout << "Root detected" << std::endl;
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
                std::cout << "Autoindex detected" << std::endl;
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
                std::cout << "index detected" << std::endl;
                route->setIndex(args);
            } catch (std::exception &e) {
                delete route;
                route = NULL;
                std::cerr << "index : " << e.what() << std::endl;
                throw ParseConfig::ErrorFileContent();
            }
        }
        else if (args.size() == 1 && args[0] == "}")
        {
            std::cout << "Config de la route " << route->getPath() << std::endl;
            std::vector<Method> m = route->getMethods();
            std::cout << "Methods : " << std::endl;
            for (size_t i = 0; i < m.size(); i++)
                std::cout << m[i] << std::endl;
            std::cout << "Redirection : " << std::endl;
            if (route->getRedir().exist)
            {
                std::cout << route->getRedir().code << std::endl;
                std::cout << route->getRedir().path << std::endl;
            }
            else
                std::cout << "no redirection" << std::endl;
            std::cout << "Root : " << route->getRoot() << std::endl;
            std::cout << "Autoindex : " << route->getAutoIndex() << std::endl;
            std::cout << "Index : " << route->getIndex() << std::endl;
            return (route);
        }
        else
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
        delete this->_configs[i];
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
