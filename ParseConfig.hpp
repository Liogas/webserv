/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:52:16 by glions            #+#    #+#             */
/*   Updated: 2025/03/18 11:14:50 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <iostream>
# include <exception>

# include "utils.hpp"
# include "ServerConfig.hpp"

class ParseConfig
{
    public:
        ParseConfig(std::string path);
        ~ParseConfig();
        ParseConfig &operator=(const ParseConfig &copy);
        std::string getPath(void) const;
        std::vector<ServerConfig *> getConfigs(void) const;
        bool startParsing();
        ServerConfig *parseServer(size_t *i, std::vector<std::string> lines);
        Route *parseRoute(size_t *i, std::vector<std::string> lines);
    private:
        std::string _path;
        std::ifstream _file;
        std::vector<ServerConfig *> _configs;

    class ErrorFile : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorFileExtension : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    
    class ErrorFileContent : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorFileEmpty : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif