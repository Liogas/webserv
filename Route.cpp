#include "Route.hpp"

Route::Route(std::string path) : _path(path)
{
    std::cout << "[Route] " << this->_path << " created" << std::endl;
}

Route::~Route()
{
    std::cout << "[Route] " << this->_path << " detroyed" << std::endl;
}

void Route::setDefaultFile(std::string path)
{
    this->_default_file = path;
}

void Route::setDirectoryListing(bool value)
{
    this->_directory_listing = value;
}

void Route::setDirectoryPath(std::string path)
{
    this->_directory_path = path;
}

void Route::setMethodHTTP(std::set<Method> methods)
{
    this->_methodsHTTP = methods;
}

void Route::setRedirectionHTTP(std::string newUrl)
{
    this->_redirectionHTTP = newUrl;
}

std::string Route::getPath(void)
{
    return (this->_path);
}

std::string Route::getRedirectionHTTP(void)
{
    return (this->_redirectionHTTP);
}

std::string Route::getDefaultFile(void)
{
    return (this->_default_file);
}

std::string Route::getDirectoryPath(void)
{
    return (this->_directory_path);
}

bool Route::getDirectoryListing(void)
{
    return (this->_directory_listing);
}

std::set<Method> Route::getMethods(void)
{
    return (this->_methodsHTTP);
}