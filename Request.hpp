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
# include "Route.hpp"
# include "ParseRequest.hpp"

class Routes;
class Server;
class Client;
class ParseRequest;

class Request {
    private:
    
    t_header    _headerInfo;
    std::string _body;
    Route       *_route;
    Server      *_server;
    Client      *_client;
    std::string _saveRequest;
    std::string _finalPath;

    std::string _htmlContent;
    std::string _dataPost;
    std::string _nameFile;
    std::map<std::string, std::string> dataForm;

public:
        //Constructors
        Request(ParseRequest *parseReq);

        //Destructor
        ~Request();

        //Operator Loaded

        //Getters
        //Setter

        //Public Methods
        void                        print(void);
        void                        handleRequest(void);
        std::string                 checkRequest(Route *route);
        void                        getInfoForm(void);
        void                        sendFileToServ(std::string boundary);
        std::string                 generateHtmlPage(std::string title, std::string body);
        std::string                 printDataForm();
        void                        startPost(void);
        std::string                 readRequest();
        std::string                 getContentFile(std::string &boundary);
        std::string                 listDir(Route *route);
        void                        sendResponse(std::string htmlContent);
        void                        initFinalPath(Route *route);
        void                        selectMethod(Route *route);
};

