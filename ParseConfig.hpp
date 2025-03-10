/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:52:16 by glions            #+#    #+#             */
/*   Updated: 2025/03/10 15:21:42 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <iostream>
# include <exception>

# include "utils.hpp"

class ParseConfig
{
    public:
        ParseConfig(std::string path);
        ~ParseConfig();
        void startParsing(void);
        bool interpretArgs(std::vector<std::string> args);
        bool interpretOnRoot(std::vector<std::string> args);
        bool interpretOnServer(std::vector<std::string> args);
    private:
        std::string _path;
        std::ifstream _file;
        bool _blocServer;
        bool _blocRoot;

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