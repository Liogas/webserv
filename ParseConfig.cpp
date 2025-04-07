/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/04/07 09:31:28 by glions           ###   ########.fr       */
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
}

ParseConfig &ParseConfig::operator=(const ParseConfig &copy)
{
    if (this != &copy)
    {
    }
    return (*this);
}

/*
    Methode qui lance parsing.
    Lit le contenu present dans le fichier de configuration, puis parcoure les lignes.
    Son role est de chercher une ligne "server {" afin de commencer le parsing pour un
    serveur. Une fois parsingServer fait on recupere l'objet ServerConfig qui a ete creer
    pour l'ajouter au vector _configs.
*/
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

/*
    Methode qui s'occupe du parsing d'un bloc "server {}", retourne un objet ServerConfig.
    Les lignes acceptees sont :
    - "listen arg" -> definit le port
    - "server_name arg" -> definir le nom de domaine
    - "error_pages code path" -> Ajoute une page d'erreur pour le code donne
    - "client_max_body value" -> definit la taille maximum d'un body envoye par le client
    - "location path {" -> Demarre le parsing d'une route
    - "}" -> Fin du bloc "server {}"
*/
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

/*
    Methode qui s'occupe du parsing du bloc "location arg {}", retourne un objet Route
    Les lignes acceptees sont :
    - "methods" : definit les methodes acceptees pour cette route
    - "return code path" : redirige cette route vers une autre requete avec un code precis
    - "root path" -> definit le repertoire de travail de la route
    - "autoindex on/off" -> definit s'il faut lister ou non le contenu du repertoire
    - "index path" -> definit le fichier par defaut a afficher
    - "}" -> Fin du bloc "location path {}"
*/
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


// GETTERS
std::vector<ServerConfig *> ParseConfig::getConfigs(void) const
{
    return (this->_configs);
}

std::string ParseConfig::getPath(void) const
{
    return (this->_path);
}


// EXCEPTIONS
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
