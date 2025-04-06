#include "Request.hpp"
#include <string>

//Constructors
Request::Request(std::string buffer, ssize_t bytes, Server *serv, Client *client) :
    _request(),
    _finalPath(),
    _htmlContent(),
    _buffer(buffer),
    _copyRequest(),
    _saveRequest(),
    _contentLength(),
    _boundary(),
    _dataPost(),
    _nameFile(),
    _method(),  
    _version(),
    _hostName(),
    _header(),
    _body(),
    _bytesRead(bytes),
    _contentLength2(-1),
    _url(),
    _contentType(),
    _agent(),
    _connection(),
    _transferEncoding(),
    _accept(),
    _route(NULL),
    _server(serv),
    _client(client)
{

}

//Destructor
Request::~Request(){

}

void Request::addBuffer(std::string buffer, ssize_t bytes)
{
    this->_buffer.append(buffer, this->_buffer.size(), bytes);
}

std::string Request::getBuffer(void) const
{
    return (this->_buffer);
}

int Request::getContentLength2(void) const
{
    return (this->_contentLength2);
}

int Request::getBytes(void) const
{
    return (this->_bytesRead);
}

void Request::setBytes(int value)
{
    this->_bytesRead = value;
}

void Request::setContentLength2(int value)
{
    this->_contentLength2 = value;
}

void Request::setHeader(std::string header)
{
    this->_header = header;
}

void Request::setBody(std::string body)
{
    this->_body = body;
}

void Request::print(void)
{
    std::cout << "Method             : " << this->_method << std::endl;
    std::cout << "Url                : " << this->_url << std::endl;
    std::cout << "Version            : " << this->_version << std::endl;
    std::cout << "Host               : " << this->_hostName << std::endl;
    std::cout << "User_Agent         : " << this->_agent << std::endl;
    std::cout << "Accept             : " << this->_accept << std::endl;
    std::cout << "AcceptLanguage     : " << this->_acceptLanguage << std::endl;
    std::cout << "AcceptEncoding     : " << this->_acceptEncoding << std::endl;
    std::cout << "Connection         : " << this->_connection << std::endl;
    if (this->_contentLength2 != -1)
        std::cout << "Content-Length     : " << this->_contentLength2 << std::endl;
    if (this->_transferEncoding)
        std::cout << "Transfert-Encoding : true" << std::endl;
    if (!   this->_contentType.empty())
        std::cout << "ContentType        : " << this->_contentType << std::endl;
    if (!this->_boundary.empty())
        std::cout << "Boundary           : " << this->_boundary << std::endl;
    std::cout << std::endl;
    std::cout << "----- HEADER -----" << std::endl;
    std::cout << this->_header << std::endl << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "----- BODY -----" << std::endl;
    std::cout << this->_body << std::endl << std::endl;
    std::cout << "----------------" << std::endl;
}

int Request::parseRequest(std::string request)
{
    std::stringstream ss(request);
    std::string token;
    
    while (ss >> token){
        if (token == "GET")
            this->_method = GET;
        else if (token == "POST")
            this->_method = POST;
        else if (token == "DELETE")
            this->_method = DELETE;
        else if (token[0] == '/')
            this->_url = token;
        else if (token == "HTTP/1.1")
            this->_version = token;
        else
            return (400);
    }
    if (this->_url.size() > 8192)
        return (414);
    this->_route = this->findLocation();
    if (this->_route == NULL)
        return (404);
    std::vector<Method> methods = this->_route->getMethods();
    std::vector<Method>::iterator it = std::find(methods.begin(), methods.end(), this->_method);
    if (it == this->_route->getMethods().end())
        return (405);
    if (this->_version != "HTTP/1.1")
        return (505);
    return (0);
}

int Request::parseHost(std::string host)
{
    std::vector<std::string> args = splitString(host, ' ');
    if (args.size() != 2)
        return (400);
    args = splitString(args[1], ':');
    if (args[0] != this->_server->getConfig()->getServerName())
        return (400);
    this->_hostName = args[0];
    return (0);
}

int Request::parseHeader(void)
{
    std::string line;
    std::istringstream stream(this->_header);
    int error;
    if (std::getline(stream, line) && (error = parseRequest(line)) != 0)
    {
        std::cerr << "ERROR REQUEST LINE -> " << error << std::endl;
        return (error);
    }
    while (std::getline(stream, line))
    {
        if (!line.compare(0, 5, "Host:") && (error = parseHost(line)) != 0)
            return (error);
        else if (!line.compare(0, 7, "Accept:"))
            this->_accept = line.substr(8);
        else if (!line.compare(0, 11, "User-Agent:"))
            this->_agent = line.substr(12);
        else if (!line.compare(0, 11, "Connection:"))
            this->_connection = line.substr(12);
        else if (!line.compare(0, 18, "Transfer-Encoding:"))
            this->_transferEncoding = true;
        else if (!line.compare(0, 16, "Accept-Language:"))
            this->_acceptLanguage = line.substr(17); 
        else if (!line.compare(0, 16, "Accept-Encoding:"))
            this->_acceptEncoding = line.substr(17);
        else if (!line.compare(0, 13, "Content-Type:"))
        {
            std::vector<std::string> args = splitString(line, ';');
            this->_contentType = args[0].substr(14, args[1].size() - 1);
            if (args.size() == 2)
                this->_boundary = args[1].substr(10);
        }
    }
    if (this->_transferEncoding && this->_contentLength2 != -1)
        return (400);
    return (0);
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
        if (name == ".." && this->_request == routeName)
            continue ;
        std::string link;
        if (this->_request.at(this->_request.size() - 1) != '/')
            link = this->_request + "/" + name;
        else
            link = this->_request + name;
        std::string tmp = "<li><a href=\"" + link + "\">" + name + "</a></li>";
        html += tmp;
    }
    html += "</ul></body></html>";
    closedir(dir);
    return (html);
}


void    Request::getInfoForm(std::istringstream &stream){
    std::vector<std::string> tmp;
    std::vector<std::string>::iterator it;
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


void    Request::startPost(std::istringstream &stream){
    if (this->_contentType == "application/x-www-form-urlencoded")
        this->getInfoForm(stream);

    else if (this->_contentType == "multipart/form-data")
    {
        std::vector<std::string> tmp = splitString(this->_boundary, '=');
        this->sendFileToServ(stream, tmp[1]);
    }
}

void    Request::getLengthContent(std::string &line){
    std::stringstream ss(line);
    std::string key;
    if (getline(ss, key, ' ')){
        std::getline(ss, this->_contentLength);
    }
}

void    Request::sendFileToServ(std::istringstream& stream, std::string boundary){
    std::string line;
    boundary = "--" + boundary;
    while (getline(stream, line))
    {
        if (line == boundary + "--") 
            break ;
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
            {
                this->_htmlContent = this->generateHtmlPage("Upload Failed",
                    "<h1>Upload of file " + this->_nameFile + " error ! </h1>");               
            }
        }
    }
}

std::string Request::getContentFile(std::string &boundary)
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
        }
    }
    return ("");
}

void    Request::getInfoPost(void){ 
    std::string line;
    std::istringstream stream(this->_buffer);

    while (getline(stream, line))
    {
        line[line.size() - 1] = '\0';
        if (strncmp("Content-Length", line.c_str(), 14) == 0)
        {
            if (!this->_contentLength.empty())
                break ;
            this->getLengthContent(line);
        }
        else if (strncmp("Content-Type", line.c_str(), 12) == 0){
            if (!this->_contentType.empty())
                break ;
            std::vector<std::string> args = splitString(line, ' ');
            this->_contentType = args[1];
            if (this->_contentType.at(this->_contentType.size() - 1 == '\0'))
                this->_contentType.resize(this->_contentType.size() - 1);
            std::cout << "Content-Type -> " << this->_contentType << std::endl;
            if (args.size() == 3)
                this->_boundary = args[2];
        }
    }
}

void    Request::selectMethod(std::istringstream &stream, Route *route){

    if (this->_method == POST)
        (this->getInfoPost(), this->startPost(stream));
    else if (this->_method == GET)
        this->_htmlContent = this->checkRequest(route);
    else if (this->_method == DELETE)
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

    this->_copyRequest = this->_url;
    std::vector<std::string> rootToken = doSplit(this->_url, '/');
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
        for (size_t i = 0; i < rootToken.size() - 1; ++i)
            tmp += rootToken[i];
        this->_copyRequest = tmp;
        this->_saveRequest.insert(0, rootToken[rootToken.size() - 1]);
        rootToken.pop_back();
    }
    if (it == tmp.end())
        return (NULL);
    return (it->second);
}



void    Request::getInfoRequest(std::string &line){
    std::stringstream ss(line);
    std::string token;

    while (ss >> token){
        if (token == "GET")
            this->_method = GET;
        else if (token == "POST")
            this->_method = POST;
        else if (token == "DELETE")
            this->_method = DELETE;
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
        strerror(errno);
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
    std::cout << this->_buffer << std::endl;
    if (std::getline(stream, line)){
        this->getInfoRequest(line);
        if (this->_version != "HTTP/1.1")
            return ; //code erreur qui correspond a la version
        if ((ptr = this->findLocation()) != NULL)
        {
            std::vector<Method> tmp = ptr->getMethods();
            if (std::find(tmp.begin(), tmp.end(), this->_method) == tmp.end())
            {
                return ; // Code d'erreur method pas accepte
            }
            this->initFinalPath(ptr);
            this->selectMethod(stream, ptr);
            if (!this->_htmlContent.empty() || this->_htmlContent == "")
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
            // ERROR 400
            this->_finalPath = "test/error/404.html";
            this->_htmlContent = this->readRequest();
            this->sendResponse(this->_htmlContent);
        }
    }
}
