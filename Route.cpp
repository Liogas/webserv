/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 09:21:24 by glions            #+#    #+#             */
/*   Updated: 2025/03/17 16:21:05 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"

Route::Route(void):
    _path(),
    _root(),
    _index(),
    _bodysize(),
    _methods(),
    _error_pages(),
    _redir()
{
    this->_redir.exist = false;
    std::cout << "[Route] created" << std::endl;
}

Route::Route(std::string path):
    _path(path),
    _root(),
    _index(),
    _bodysize(),
    _methods(),
    _error_pages(),
    _redir()
{
    this->_redir.exist = false;
    std::cout << "[Route] created with " << this->_path << std::endl;
}

Route::Route(const Route &copy):
    _path(copy.getPath()),
    _root(copy.getRoot()),
    _index(copy.getIndex()),
    _bodysize(copy.getBodySize()),
    _methods(copy.getMethods()),
    _error_pages(copy.getErrorPages()),
    _redir(copy.getRedir())
{
    std::cout << "[Route] copy constructor called" << std::endl;
}

Route::~Route()
{
    std::cout << "[Route] destroyed" << std::endl;
}

// SETTERS
void Route::setAutoIndex(bool value)
{
    this->_autoindex = value;
}

void Route::setAutoIndex(std::vector<std::string> args)
{
    if (args.size() > 2)
        throw Route::ErrorToManyArgs();
    if (args.size() < 2)
        throw Route::ErrorNotEnoughArgs();
    if (args[1] == "on")
        this->_autoindex = true;
    else if (args[1] == "off")
        this->_autoindex = false;
    else
        throw Route::ErrorNotValidArgs();
}

void Route::setBodySize(int size)
{
    this->_bodysize = size;
}

void Route::setPath(std::string path)
{
    this->_path = path;
}

void Route::setIndex(std::string index)
{
    this->_index = index;
}

void Route::setIndex(std::vector<std::string> args)
{
    if (args.size() > 2)
        throw Route::ErrorToManyArgs();
    if (args.size() < 2)
        throw Route::ErrorNotEnoughArgs();
    this->_index = args[1];
}

void Route::setRoot(std::string root)
{
    this->_root = root;
}

void Route::setRoot(std::vector<std::string> args)
{
    if (args.size() > 2)
        throw Route::ErrorToManyArgs();
    if (args.size() < 2)
        throw Route::ErrorNotEnoughArgs();
    this->_root = args[1];
}

void Route::addErrorPage(int error, std::string path)
{
    this->_error_pages[error] = path;
}

void Route::addMethod(Method m)
{
    this->_methods.push_back(m);
}

bool alreadyIn(Method m, std::vector<Method> tab)
{
    for (size_t i = 0; i < tab.size(); i++)
    {
        if (tab[i] == m)
            return (true);
    }
    return (false);
}

void Route::addMethods(std::vector<std::string> args)
{
    if (args.size() < 2)
    {
        throw Route::ErrorNotEnoughArgs();
    }
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i] == "GET" && !alreadyIn(GET, this->_methods))
            this->_methods.push_back(GET);
        else if (args[i] == "POST" && !alreadyIn(POST, this->_methods))
            this->_methods.push_back(POST);
        else if (args[i] == "DELETE" && !alreadyIn(DELETE, this->_methods))
            this->_methods.push_back(DELETE);
        else
            throw Route::ErrorNotValidArgs();
    }
}

void Route::setRedir(t_redirection redir)
{
    this->_redir = redir;
}

void Route::setRedir(std::vector<std::string> args)
{
    if (args.size() > 3)
        throw Route::ErrorToManyArgs();
    if (args.size() < 3)
        throw Route::ErrorNotEnoughArgs();
    for (size_t i = 0; i < args[1].size(); i++)
    {
        if (!isdigit(args[1].at(i)))
            throw Route::ErrorNotValidArgs();
    }
    std::istringstream iss(args[1]);
    int num;
    if (!(iss >> num))
        throw Route::ErrorNotValidArgs();
    this->_redir.exist = true;
    this->_redir.code = num;
    this->_redir.path = args[2];
}

// GETTERS
std::string Route::getPath(void) const
{
    return (this->_path);
}

std::string Route::getRoot(void) const
{
    return (this->_root);
}

std::string Route::getIndex(void) const
{
    return (this->_index);
}

int Route::getBodySize(void) const
{
    return (this->_bodysize);
}

bool Route::getAutoIndex(void) const
{
    return (this->_autoindex);
}

std::vector<Method> Route::getMethods(void) const
{
    return (this->_methods);
}

std::map<int, std::string> Route::getErrorPages(void) const
{
    return (this->_error_pages);
}

t_redirection Route::getRedir(void) const
{
    return (this->_redir);
}

const char *Route::ErrorNotEnoughArgs::what() const throw()
{
    return ("[!Route!] not enough args");
}

const char *Route::ErrorToManyArgs::what() const throw()
{
    return ("[!Route!] to much args");
}

const char *Route::ErrorNotValidArgs::what() const throw()
{
    return ("[!Route!] not valid args");
}
