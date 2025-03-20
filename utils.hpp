/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:49:02 by glions            #+#    #+#             */
/*   Updated: 2025/03/20 14:57:33 by glions           ###   ########.fr       */
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

// MANIP STRING
std::vector<std::string> splitString(const std::string str, char sep);
bool isValidExtension(const std::string &filename, const std::string &extension);
std::vector<std::string> readFile(std::ifstream &file);
void cleanArgs(std::vector<std::string> *args);

#endif