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
