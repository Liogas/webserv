#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <set>
# include <iostream>

# include "utils.hpp"

class Route
{
    public:
        Route(std::string path);
        ~Route();
        void setMethodHTTP(std::set<Method> methods);
        void setRedirectionHTTP(std::string newUrl);
        void setDirectoryPath(std::string path);
        void setDirectoryListing(bool val);
        void setDefaultFile(std::string path);
        std::string getPath(void);
        std::string getRedirectionHTTP(void);
        std::string getDirectoryPath(void);
        bool getDirectoryListing(void);
        std::string getDefaultFile(void);
        std::set<Method> getMethods(void);
    private:
        std::set<Method> _methodsHTTP;
        std::string _path;
        std::string _redirectionHTTP;
        std::string _directory_path;
        bool _directory_listing;
        std::string _default_file;
        
};

#endif