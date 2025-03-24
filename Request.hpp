#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <sys/stat.h> //pour stat
# include <dirent.h>   // Pour opendir, readdir et closedir

# include "Server.hpp"

class Routes;

class Request {
    private:
    std::string _method;
    std::string _request;
    std::string _version;
    std::string _hostName;
    std::string _finalPath;
    std::string _htmlContent;
    std::string _buffer;
    std::string _copyRequest;
    std::string _saveRequest;
    Server      *_server;
    Client      *_client;
public:
        //Constructors
        Request(std::string buffer, Server *serv, Client *client);

        //Destructor
        ~Request();

        //Operator Loaded

        //Getters

        //Setter

        //Public Methods
        void                        handleRequest(void);
        void                        getInfoRequest(std::string &line);
        Route                      *findLocation();
        void                        checkRequest(Route *route);
        std::string                 readRequest();
        std::vector<std::string>    doSplit(const std::string& str, char delimiter);
        void                        sendResponse(std::string htmlContent);
        void                        initFinalPath(Route *route);
};

