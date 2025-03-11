/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:49:02 by glions            #+#    #+#             */
/*   Updated: 2025/03/11 09:32:05 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <vector>
# include <sstream>
# include <fstream>

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