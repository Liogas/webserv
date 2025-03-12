/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:52:16 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 11:19:47 by glions           ###   ########.fr       */
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
        void startParsing(void);
        bool interpretArgs(std::vector<std::string> args);
        bool interpretOnRoute(std::vector<std::string> args);
        bool interpretOnServer(std::vector<std::string> args);
        bool interpretListen(std::vector<std::string> args);
        bool interpretServerNames(std::vector<std::string> args);
        ParseConfig &operator=(const ParseConfig &copy);
        std::string getPath(void) const;
        bool getBlocServer(void) const;
        bool getBlocRoute(void) const;
        ServerConfig getConfig(void) const;
    private:
        std::string _path;
        std::ifstream _file;
        bool _blocServer;
        bool _blocRoute;
        ServerConfig *_config;

    class ErrorExtension : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorFile : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
    class ErrorFileContent : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif