/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 09:21:24 by glions            #+#    #+#             */
/*   Updated: 2025/03/17 11:39:58 by glions           ###   ########.fr       */
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

void Route::setRoot(std::string root)
{
    this->_root = root;
}

void Route::addErrorPage(int error, std::string path)
{
    this->_error_pages[error] = path;
}

void Route::addMethod(Method m)
{
    this->_methods.push_back(m);
}

void Route::setRedir(t_redirection redir)
{
    this->_redir = redir;
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
