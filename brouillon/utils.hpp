#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <vector>
# include <sstream>

enum Method
{
    GET,
    POST,
    DELETE,
    UNKNOWN
};

// MANIP STRING
std::vector<std::string> splitString(const std::string str, char sep);
void cleanArgs(std::vector<std::string> &args);

#endif