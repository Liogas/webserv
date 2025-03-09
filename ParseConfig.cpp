/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:54:05 by glions            #+#    #+#             */
/*   Updated: 2025/03/09 20:26:53 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParseConfig.hpp"

ParseConfig::ParseConfig(std::string path) : _path(path)
{
    if (!isValidExtension(this->_path, "conf"))
        throw ParseConfig::ErrorExtension();
    std::cout << "[ParseConfig] created with " << this->_path << std::endl;
    this->_file.open(this->_path.c_str());
    if (!this->_file.is_open())
        throw ParseConfig::ErrorFile();
}

ParseConfig::~ParseConfig(void)
{
    if (this->_file.is_open())
    {
        std::cout << "je rentre ici" << std::endl;        
        this->_file.close();
    }
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