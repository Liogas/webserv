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
    std::string _method;
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
        void                        handleRequest(void);
        void                        getInfoRequest(std::string &line);
        Route                      *findLocation();
        void                        checkRequest(Route *route);
        void                        selectMethod(void);
        void                        getLengthContent(std::string &line);
        void                        parsePost(void);
        void                        parsePostForm(std::istringstream& stream);
        void                        parseDataPost(std::string &str);
        void                        sendFileToServ(std::istringstream& stream, std::string boundary);
        void                        generateHtmlPage();
        std::string                 printDataForm();
        void                        startPost(void);
        std::string                 readRequest();
        std::string                 getContentFile(std::string boundary);
        std::vector<std::string>    doSplit(const std::string& str, char delimiter);
        void                        listDir(Route *route);
        void                        sendResponse(std::string htmlContent);
        void                        initFinalPath(Route *route);
};

