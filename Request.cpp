#include "Request.hpp"
#include <string>

//Constructors

Request::Request(ParseRequest *parseReq) :
    _headerInfo(parseReq->getHeaderInfo()),
    _body(parseReq->getBody()),
    _route(parseReq->getRoute()),
    _server(parseReq->getServer()),
    _client(parseReq->getClient()),
    _finalPath()
{
}

//Destructor
Request::~Request(){

}

void Request::print(void)
{
    std::cout << "Method             : " << this->_headerInfo.method << std::endl;
    std::cout << "Url                : " << this->_headerInfo.url << std::endl;
    std::cout << "Version            : " << this->_headerInfo.version << std::endl;
    std::cout << "Host               : " << this->_headerInfo.hostName << std::endl;
    std::cout << "User_Agent         : " << this->_headerInfo.agent << std::endl;
    std::cout << "Accept             : " << this->_headerInfo.accept << std::endl;
    std::cout << "AcceptLanguage     : " << this->_headerInfo.acceptLanguage << std::endl;
    std::cout << "AcceptEncoding     : " << this->_headerInfo.acceptEncoding << std::endl;
    std::cout << "Connection         : " << this->_headerInfo.connection << std::endl;
    if (this->_headerInfo.contentLength != -1)
        std::cout << "Content-Length     : " << this->_headerInfo.contentLength << std::endl;
    if (this->_headerInfo.transferEncoding)
        std::cout << "Transfert-Encoding : true" << std::endl;
    if (!   this->_headerInfo.contentType.empty())
        std::cout << "ContentType        : " << this->_headerInfo.contentType << std::endl;
    if (!this->_headerInfo.boundary.empty())
        std::cout << "Boundary           : " << this->_headerInfo.boundary << std::endl;
    std::cout << "Route :" << std::endl;
    this->_route->print();
    std::cout << std::endl;
    std::cout << "----- BODY -----" << std::endl;
    std::cout << this->_body << std::endl;
    std::cout << "----------------" << std::endl;
}

/* **************************** */

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

std::string Request::listDir(Route *route)
{
    DIR *dir;
    struct dirent *entry;
    std::string html;
    std::vector<struct dirent *> entrys;
    dir = opendir(this->_finalPath.c_str());
    if (!dir)
    {
        std::cerr << "ERROR open directory" << std::endl;
        return "";
    }
    html = "<html><body><h1>Contents :</h1><ul>";
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        std::string routeName = route->getPath();
        if (routeName.at(routeName.size() - 1) != '/')
            routeName.insert(routeName.size(), "/");
        if (name == ".." && this->_headerInfo.url == routeName)
            continue ;
        std::string link;
        if (this->_headerInfo.url.at(this->_headerInfo.url.size() - 1) != '/')
            link = this->_headerInfo.url + "/" + name;
        else
            link = this->_headerInfo.url + name;
        std::string tmp = "<li><a href=\"" + link + "\">" + name + "</a></li>";
        html += tmp;
    }
    html += "</ul></body></html>";
    closedir(dir);
    return (html);
}


void    Request::getInfoForm(void){
    std::vector<std::string> tmp;
    std::vector<std::string>::iterator it;
    std::string body = this->_body;
    size_t startBody = body.find("\r\n\r\n");
    if (startBody != std::string::npos)
    {
        startBody += 4;
        body.erase(0, startBody);
    }
    std::istringstream stream(body);
    //------------------------- TROUVER LA LIGNE CONTENANT DATA
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
//------------------ PARSER LA LIGNE POUR RECUPERER LES DONNEES FORM
    tmp = splitString(this->_dataPost, '&');
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

std::string Request::generateHtmlPage(std::string title, std::string body) {

    std::string htmlContent;
    htmlContent += "<!DOCTYPE html>\n";
    htmlContent += "<html lang=\"en\">\n";
    htmlContent += "<head>\n";
    htmlContent += "<meta charset=\"UTF-8\">\n";
    htmlContent += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    htmlContent += "<title>" + title + "</title>\n";
    htmlContent += "</head>\n";
    htmlContent += "<body>\n";
    htmlContent += body;
    htmlContent += "</body>\n";
    htmlContent += "</html>\n";
    return (htmlContent);
}


void    Request::startPost(){
    if (this->_headerInfo.contentType == "application/x-www-form-urlencoded")
        this->getInfoForm();

    else if (this->_headerInfo.contentType == "multipart/form-data")
        this->sendFileToServ(this->_headerInfo.boundary);
}

void    Request::sendFileToServ(std::string boundary){
    std::cout << "Valeur du boundary : " << boundary << std::endl;
    std::string line;
    boundary = "--" + boundary;
    std::istringstream stream(this->_body);
    while (getline(stream, line))
    {
        if (line == boundary + "--") 
        {
            std::cout << "Boundary detected" << std::endl;
            break ;
        }
        // Pour recuperer le nom du fichier a envoyer sur le serveur
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
        if (this->_finalPath.at(this->_finalPath.size() - 1) != '/')
            this->_finalPath.insert(this->_finalPath.size(), "/");
        this->_nameFile = this->_finalPath + this->_nameFile; // ptetre changer finalpath par un dossier par defaut
        if (!body.empty()){
            std::ofstream htmlFile(this->_nameFile.c_str(), std::ios::binary);
            if (htmlFile.is_open())
            {
                htmlFile.write(body.c_str(), body.size());
                htmlFile.close();
                this->_htmlContent = this->generateHtmlPage("Upload Success",
                    "<h1>File " + this->_nameFile + " uploaded ! </h1>");               
            }
            else
                this->_htmlContent = this->generateHtmlPage("Upload Failed",
                    "<h1>Upload of file " + this->_nameFile + " error ! </h1>");               
        }
    }
}
std::string Request::getContentFile(std::string &boundary)
{
    size_t startBody = this->_body.find("\r\n\r\n") + 4;
    if (startBody != std::string::npos)
    {
        this->_body.erase(0, startBody);
        size_t end = this->_body.find(boundary.c_str());
        std::cout << "END : " << end << std::endl;
        if (end != std::string::npos)
        {
            std::cout << "JE SORS ICI" << std::endl;
            return (this->_body.substr(0, end));
        }
    }
    return (this->_body);
}

void    Request::selectMethod(Route *route){
    if (this->_headerInfo.method == POST)
        this->startPost();
    else if (this->_headerInfo.method == GET)
        this->_htmlContent = this->checkRequest(route);
    else if (this->_headerInfo.method == DELETE)
        std::cout << "DELETE PAS ENCORE GERE" << std::endl;
}

std::string     Request::checkRequest(Route *route){

    if (!route)
        return "";
    if (isFile(this->_finalPath))
        return (this->readRequest());
    else if (isDirectory(this->_finalPath))
    {
        if (!route->getIndex().empty()){
            if (this->_finalPath[this->_finalPath.size() - 1] == '/')
                this->_finalPath += route->getIndex();
            else
                this->_finalPath += "/" + route->getIndex();
            return (this->readRequest());
        }
        else if (route->getAutoIndex() == 1)
            return (this->listDir(route));
    }
    return ("");
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
        strerror(errno);
        this->_client->disconnect();
        delete this->_client;
        this->_client = NULL;
    }
}

void    Request::handleRequest(void){
    if (this->_route != NULL)
    {
        std::cout << "(1)" << std::endl;
        this->initFinalPath(this->_route);
        std::cout << "Final path : " << this->_finalPath << std::endl;
        this->selectMethod(this->_route);
        if (!this->_htmlContent.empty() || this->_htmlContent == "")
        {
            std::cout << "HTMLCONTENT VAUT : " << std::endl;
            std::cout << this->_htmlContent << std::endl;
            std::cout << "(2)" << std::endl;
            this->sendResponse(this->_htmlContent);
        }
        else
        {
            this->_finalPath = "test/error/404.html";
            this->_htmlContent = this->readRequest();
            std::cout << "(3)" << std::endl;
            this->sendResponse(this->_htmlContent);
        }
    }
    else
    {
        // ERROR 400
        this->_finalPath = "test/error/404.html";
        this->_htmlContent = this->readRequest();
        std::cout << "(4)" << std::endl;
        this->sendResponse(this->_htmlContent);
    }
}
