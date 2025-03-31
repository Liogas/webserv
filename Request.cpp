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

void Request::listDir(Route *route)
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


void    Request::parseDataPost(std::string &str){
    std::vector<std::string> tmp;
    std::vector<std::string>::iterator it;

    tmp = splitString(str, '&');
    for (it = tmp.begin(); it != tmp.end(); ++it){
        std::stringstream ss(*it);
        std::string key;
        std::string value;
        std::getline(ss, key, '=');
        std::getline(ss, value);
        this->dataForm[key] = value;
    }
}

std::string Request::printDataForm(){
  std::string tmp;
  std::map<std::string, std::string>::iterator it;

    tmp = "<ul>";
    for(it = this->dataForm.begin(); it != this->dataForm.end(); ++it){
        tmp += "<li>" + it->first + ": " + it->second + "</li>";
    }
    tmp += "</ul>";
    return (tmp);
}

void Request::generateHtmlPage() {
    std::ofstream htmlFile(this->_finalPath.c_str());

    if (htmlFile.is_open()) {
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html lang=\"en\">\n";
        htmlFile << "<head>\n";
        htmlFile << "<meta charset=\"UTF-8\">\n";
        htmlFile << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        htmlFile << "<title>Page Personnalisée</title>\n";
        htmlFile << "</head>\n";
        htmlFile << "<body>\n";
        htmlFile << "<h1>Bonjour, " << "Donnees du formulaire:" << "</h1>\n";
        htmlFile << this->printDataForm();
        htmlFile << "</body>\n";
        htmlFile << "</html>\n";

        htmlFile.close();
        std::cout << "La page HTML a été générée avec succès !\n";
    } else {
        std::cerr << "Impossible de créer le fichier HTML.\n";
    }
}


void    Request::startPost(){
    if (this->_contentType == "application/x-www-form-urlencoded"){ //form
            this->parseDataPost(this->_dataPost);
            this->generateHtmlPage();
    }

    else if (this->_contentType == "multipart/form-data")
    {
        std::cout <<"OK" << std::endl;
    }
}

void    Request::getLengthContent(std::string &line){
    std::stringstream ss(line);
    std::string key;
    if (getline(ss, key, ' ')){
        std::getline(ss, this->_contentLength);
    }
}

void Request::parsePostForm(std::istringstream& stream)
{
    std::string line;
    while (getline(stream, line))
    {
        if ((line.size() == 1 && line.at(0) == '\0')
            || (line.size() == 1 && line == "\r"))
        {
            getline(stream, this->_dataPost);
            break ;
        }
    }
}

void    Request::sendFileToServ(std::istringstream& stream, std::string boundary){
    std::string line;
    boundary = "--" + boundary;
    while (getline(stream, line))
    {
        // std::cout << "line esttttttttttttttttttttttt: " << line << std::endl;
       if (line == boundary + "--") 
            break ;
        if (strncmp(line.c_str(), "Content-Disposition:", 20) == 0){
            std::vector<std::string>tmp = splitString(line.c_str(), ' ');
            std::vector<std::string>tmp1 = splitString(tmp[3], '=');
            tmp1[1].erase(0,1);
            tmp1[1].erase(tmp1[1].size() - 2, tmp1[1].size() - 1);
            this->_nameFile = tmp1[1];
        }
    }
    if (!this->_nameFile.empty())
    {
        std::string body = this->getContentFile(boundary);
        this->_nameFile = this->_finalPath + this->_nameFile;
        std::cout << "nameFile -> " << this->_nameFile << std::endl;
        // std::ofstream file(this->)
    }
}
std::string Request::getContentFile(std::string boundary)
{
    size_t startBody = this->_buffer.find("\r\n\r\n") + 4;
    if (startBody != std::string::npos)
    {
        this->_buffer.erase(0, startBody);
        startBody = this->_buffer.find("\r\n\r\n") + 4;
        if (startBody != std::string::npos)
        {
            this->_buffer.erase(0, startBody);
            boundary.resize(boundary.size() - 1);
            size_t end = this->_buffer.find(boundary);
            if (end != std::string::npos)
            {
                return (this->_buffer.substr(0, end));
            }
            else
                return (this->_buffer.substr(startBody));
        }
    }
    return ("");
}
void    Request::parsePost(){
    std::string line;
    std::istringstream stream(this->_buffer);

    while (getline(stream, line))
    {
        line[line.size() - 1] = '\0';
        if (strncmp("Content-Length", line.c_str(), 14) == 0)
            this->getLengthContent(line);
        else if (strncmp("Content-Type", line.c_str(), 12) == 0){
            std::cout << "line -> " << line << std::endl;
            std::vector<std::string> args = splitString(line, ' ');
            this->_contentType = args[1];
            if (this->_contentType.at(this->_contentType.size() - 1 == '\0'))
                this->_contentType.resize(this->_contentType.size() - 1);
            // std::cout << "CONTENT TYPE -> " << this->_contentType << std::endl;
            if (this->_contentType == "application/x-www-form-urlencoded") //OK
                this->parsePostForm(stream);
            else if (this->_contentType == "multipart/form-data") //on sait que on veut envoyer des donnees vers le serveur
            {
                std::cout << "args[2] -> " << args[2] << std::endl;
                std::vector<std::string> tmp = splitString(args[2], '=');
                std::cout << "Mon boundary = " << tmp[1] << std::endl;
                this->sendFileToServ(stream, tmp[1]);
            }
            else if (this->_contentType == "application/json")
            {
                // JSON
                std::cout << "WIP" << std::endl;
            }
            else if (this->_contentType == "application/xml")
            {
                // XML
                std::cout << "WIP" << std::endl;
            }
            else if (this->_contentType == "text/plain")
            {
                // TEXTE BRUT
                std::cout << "WIP" << std::endl;
            }
            else if (this->_contentType == "application/javascript")
            {
                // JS
                std::cout << "WIP" << std::endl;
            }
            else if (this->_contentType == "image/png" || "image/jpeg" || "image/gif" )
            {
                // IMG
                std::cout << "WIP" << std::endl;
            }

        }
    }
}

void    Request::selectMethod(){

    if (this->_method == "POST"){
        this->parsePost();
        this->startPost();
    }
    this->_htmlContent = this->readRequest();

}

void     Request::checkRequest(Route *route){

    if (!route)
        return ;
    this->initFinalPath(route);
    if (isFile(this->_finalPath))
        this->selectMethod();
    else if (isDirectory(this->_finalPath))
    {
        if (!route->getIndex().empty()){
            if (this->_finalPath[this->_finalPath.size() - 1] == '/')
                this->_finalPath += route->getIndex();
            else
                this->_finalPath += "/" + route->getIndex();
            this->selectMethod();
        }
        else if (route->getAutoIndex() == 1)
            this->listDir(route);
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
            break ;
        if (rootToken.size() <= 0)
            break ;
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
        // else
            // return ; ERREUR A GERER (TROUVER LE CODE)
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

    std::cout << "AFFICHAGE DU BUFFER" << std::endl;
    std::cout << this->_buffer << std::endl;
    if (std::getline(stream, line)){
        this->getInfoRequest(line);
        // if (!this->_request.empty()){
            if ((ptr = this->findLocation()) != NULL)
            {
                this->checkRequest(ptr);
                if (!this->_htmlContent.empty())
                {
                    this->sendResponse(this->_htmlContent);
                }
                else
                {
                    std::cout << "error" << std::endl;
                    this->_finalPath = "test/error/404.html";
                    this->_htmlContent = this->readRequest();
                    this->sendResponse(this->_htmlContent);
                }
            }
            else
            {
                // ERROR 400
                std::cout << "error (2)" << std::endl;
                this->_finalPath = "test/error/404.html";
                this->_htmlContent = this->readRequest();
                this->sendResponse(this->_htmlContent);
            }
        // }
    }
}