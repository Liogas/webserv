#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <sys/stat.h> //pour stat
#include <dirent.h>   // Pour opendir, readdir et closedir
#include <cstring>

# include "Server.hpp"

class Routes;

class Request {
    private:
    Method _method;
    std::string _request;
    std::string _version;
    std::string _hostName;
    std::string _finalPath;
    std::string _htmlContent;
    std::string _buffer;
    std::string _copyRequest;
    std::string _saveRequest;
    std::string _contentType;
    std::string _contentLength;
    std::string _boundary;
    std::string _dataPost;
    std::string _nameFile;
    Server      *_server;
    Client      *_client;
    std::map<std::string, std::string> dataForm;

public:
        //Constructors
        Request(std::string buffer, Server *serv, Client *client);

        //Destructor
        ~Request();

        //Operator Loaded

        //Getters

        //Setter

        //Public Methods
        void                        addBuffer(std::string buffer, ssize_t bytes);
        void                        handleRequest(void);
        void                        getInfoRequest(std::string &line);
        Route                      *findLocation();
        std::string                 checkRequest(Route *route);
        void                        selectMethod(std::istringstream &stream, Route *route);
        void                        getLengthContent(std::string &line);
        void                        getInfoPost(void);
        void                        getInfoForm(std::istringstream &stream);
        void                        sendFileToServ(std::istringstream& stream, std::string boundary);
        std::string                 generateHtmlPage(std::string title, std::string body);
        std::string                 printDataForm();
        void                        startPost(std::istringstream &stream);
        std::string                 readRequest();
        std::string                 getContentFile(std::string &boundary);
        std::vector<std::string>    doSplit(const std::string& str, char delimiter);
        std::string                 listDir(Route *route);
        void                        sendResponse(std::string htmlContent);
        void                        initFinalPath(Route *route);
};

