/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:49:02 by glions            #+#    #+#             */
/*   Updated: 2025/04/07 14:16:58 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include <fstream>
# include <string>
# include <algorithm>
# include <string>

# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>

enum Method
{
    GET,
    POST,
    DELETE,
    UNKNOWN
};

typedef struct s_header
{
    Method      method;
    std::string url;
    std::string version;
    std::string hostName;
    std::string agent;
    int         contentLength;
    std::string contentType;
    std::string connection;
    bool        transferEncoding;
    std::string accept;
    std::string acceptLanguage;
    std::string acceptEncoding;
    std::string boundary;
}   t_header;

// MANIP STRING
std::vector<std::string> splitString(const std::string str, char sep);
bool isValidExtension(const std::string &filename, const std::string &extension);
std::vector<std::string> readFile(std::ifstream &file);
void cleanArgs(std::vector<std::string> *args);
std::string toString(int value);

#endif