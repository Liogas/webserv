/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 19:49:21 by glions            #+#    #+#             */
/*   Updated: 2025/03/18 11:06:05 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::vector<std::string> splitString(const std::string str, char sep)
{
    std::vector<std::string> values;
    std::stringstream ss(str);
    std::string value;

    while (std::getline(ss, value, sep))
    {
        value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
        if (value.size() > 0)
            values.push_back(value);
    }
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

void cleanArgs(std::vector<std::string> *args)
{
    for (std::vector<std::string>::iterator it = args->begin(); it != args->end(); ++it)
    {
        if (it->size() == 0 || *it == "\t" || *it == "\n" || *it == "" || *it == " " || *it == "\r" || *it == "\v")
            args->erase(it);
    }
}

std::vector<std::string> readFile(std::ifstream &file)
{
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);
    return (lines);
}
