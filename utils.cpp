/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:49:21 by glions            #+#    #+#             */
/*   Updated: 2025/03/09 15:22:40 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::vector<std::string> splitString(const std::string str, char sep)
{
    std::vector<std::string> values;
    std::stringstream ss(str);
    std::string value;

    while (std::getline(ss, value, sep))
        values.push_back(value);
    return (values);
}

bool isValidExtension(const std::string &filename, const std::string &extension)
{
    bool point = false;
    for (size_t i = 0; i < filename.size(); i++)
    {
        if (point == false && filename.at(i) == '.')
            point = true;
        else if (point == true && filename.at(i) == '.')
            return (false);
    }
    std::size_t dotPos = filename.rfind('.');
    if (dotPos == std::string::npos || dotPos == 0 || dotPos == filename.length() - 1)
        return (false);
    std::string ext = filename.substr(dotPos + 1);
    if (extension == ext)
        return (true);
    return (false);
}
