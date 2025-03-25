#include "Request.hpp"

//Constructors
Request::Request(std::string buffer, Server *serv, Client *client) :
    _method(),
    _request(),
    _version(),
    _hostName(),
    _finalPath(),
    _htmlContent(),
    _buffer(buffer),
    _copyRequest(),
    _saveRequest(),
    _server(serv),
    _client(client)   
{

}

//Destructor
Request::~Request(){

}

std::string    Request::readRequest(){
    std::ifstream file(this->_finalPath.c_str());
    std::ostringstream contentStream;
    contentStream << file.rdbuf();
    return contentStream.str();
}


bool isFile(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        std::cerr << "Erreur d'accès au fichier: " << path << std::endl;
        return false;
    }
    return (info.st_mode & S_IFREG) != 0;
}

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        std::cerr << "Erreur d'accès au repertoire: " << path << std::endl;
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}


void Request::initFinalPath(Route *route)
{
    std::string tmp;
    tmp = route->getRoot();
    if (tmp.size() > 1 && tmp.at(0) == '/')
        tmp.erase(0, 1);
    if (tmp.size() == 1 && tmp.at(0) == '/')
        tmp.insert(0, ".");
    if (this->_saveRequest.size() != 0 && this->_saveRequest.at(0) != '/')
        this->_saveRequest.insert(0, "/");
    this->_finalPath = tmp + this->_saveRequest;
}

void     Request::checkRequest(Route *route){

    if (!route)
        return ;
    this->initFinalPath(route);
    if (isFile(this->_finalPath))
        this->_htmlContent = this->readRequest();
    else if (isDirectory(this->_finalPath))
    {
        if (!route->getIndex().empty()){
            if (this->_finalPath[this->_finalPath.size() - 1] == '/'){
                this->_finalPath += route->getIndex();
            }
            else
                this->_finalPath += "/" + route->getIndex();
            this->_htmlContent = this->readRequest();  
        }
        else if (route->getAutoIndex() == 1)
        {
            DIR *dir;
            struct dirent *entry;
            std::vector<struct dirent *> entrys;
            dir = opendir(this->_finalPath.c_str());
            if (!dir)
            {
                std::cout << "ERROR open directory" << std::endl;
                return ;
            }
            this->_htmlContent = "<html><body><h1>Contents :</h1><ul>";
            while ((entry = readdir(dir)) != NULL)
            {
                std::string name = entry->d_name;
                std::string routeName = route->getPath();
                if (routeName.at(routeName.size() - 1) != '/')
                    routeName.insert(routeName.size(), "/");
                if (name == ".." && this->_request == routeName)
                    continue ;
                std::string link;
                if (this->_request.at(this->_request.size() - 1) != '/')
                    link = this->_request + "/" + name;
                else
                    link = this->_request + name;
                std::string tmp = "<li><a href=\"" + link + "\">" + name + "</a></li>";
                this->_htmlContent += tmp;
            }
            this->_htmlContent += "</ul></body></html>";
            closedir(dir);
        }
    }
}

std::vector<std::string> Request::doSplit(const std::string& str, char delimiter){
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    if (str[0] == '/'){
        result.push_back("/");
        start = 1;
        end = str.find(delimiter, start); 
    }
    while (end != std::string::npos){
        result.push_back(str.substr(start, end - start));
        result.push_back("/");
        start = end + 1;
        end = str.find(delimiter, start);
    }
    if (start < str.length()) {
        result.push_back(str.substr(start));
    }
    return result;
}


Route *Request::findLocation(){
    std::string copyRequest;

    this->_copyRequest = this->_request;
    std::vector<std::string> rootToken = doSplit(this->_request, '/');
    std::map<std::string, Route *> tmp = this->_server->getConfig()->getRoutes();
    std::map<std::string, Route*>::iterator it;
    it = tmp.end();
    while (it == tmp.end()){
        it = tmp.find(this->_copyRequest);
        if (it != tmp.end() && it->first == this->_copyRequest)
        {
            std::cout << "Route trouve" << std::endl;
            break ;
        }
        if (rootToken.size() <= 0)
        {
            std::cout << "Aucune route trouvee" << std::endl;
            break ;
        }
        std::string tmp;
        for (size_t i = 0; i < rootToken.size() - 1; ++i) {
            tmp += rootToken[i];
        }
        this->_copyRequest = tmp;
        this->_saveRequest.insert(0, rootToken[rootToken.size() - 1]);
        rootToken.pop_back();
    }
    if (it == tmp.end())
    {
        return (NULL);
    }
    return (it->second);
}



void    Request::getInfoRequest(std::string &line){
    std::stringstream ss(line);
    std::string token;

    while (ss >> token){
        if (token == "GET" || token == "POST" || token == "DELETE")
            this->_method = token;
        else if (token[0] == '/')
            this->_request = token;
        else if (token == "HTTP/1.1")
            this->_version = token;
    }
}


void Request::sendResponse(std::string htmlContent){

    ssize_t content_length = htmlContent.size();
    std::string content_string = toString(content_length);
    std::string response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: " + content_string + "\r\n"
    "\r\n"
    + htmlContent;

    ssize_t bytes_sent = send(this->_client->getFd(), response.c_str(), response.size(), 0);
    if (bytes_sent < 0) {
        perror("send");
        this->_client->disconnect();
        delete this->_client;
        this->_client = NULL;
    }
}

void    Request::handleRequest(){
    std::string line;
    std::string htmlContent;
    std::istringstream stream(this->_buffer);
    Route *ptr = NULL;

    if (std::getline(stream, line)){
        this->getInfoRequest(line);
        if (!this->_request.empty()){
            if ((ptr = this->findLocation()) != NULL)
            {
                this->checkRequest(ptr);
                if (!this->_htmlContent.empty())
                    this->sendResponse(this->_htmlContent);
                else
                {
                    this->_finalPath = "test/error/404.html";
                    this->_htmlContent = this->readRequest();
                    this->sendResponse(this->_htmlContent);
                }
            }
            else
            {
                // SEND ERROR PAGE
                this->_finalPath = "test/error/404.html";
                this->_htmlContent = this->readRequest();
                this->sendResponse(this->_htmlContent);
            }
        }
    }
}