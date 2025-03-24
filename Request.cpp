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



void     Request::checkRequest(Route *route){
    std::string tmp;

    if (!route)
        std::cout << "PAS DE ROUTE" << std::endl;
    route->print();
    std::cout << std::endl;
    tmp = route->getRoot();
    if (tmp.size() > 1 && tmp.at(0) == '/')
        tmp.erase(0, 1);
    if (tmp.size() == 1 && tmp.at(0) == '/')
        tmp.insert(0, ".");
    if (this->_saveRequest.size() != 0 && this->_saveRequest.at(0) != '/')
        this->_saveRequest.insert(0, "/");
    this->_finalPath = tmp + this->_saveRequest;
    std::cout << "FINAL PATH -> " << this->_finalPath << std::endl;
    if (isFile(this->_finalPath))
        this->_htmlContent = this->readRequest();
    else if (isDirectory(this->_finalPath)){
        if (!route->getIndex().empty()){
            //ajout de l'index, vérifier si _finalPath termine par /
            if (this->_finalPath[_finalPath.size() - 1] == '/'){
                this->_finalPath += route->getIndex();
            }
            else
                this->_finalPath += "/" + route->getIndex();
            this->_htmlContent = this->readRequest();  

        }
    }
    else
        std::cout << "Aucun répertoire ni file existe" << std::endl;
    std::cout << "------------>_FinalPath is:" << _finalPath << std::endl;
}


std::vector<std::string> Request::doSplit(const std::string& str, char delimiter){
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    if (str[0] == '/'){
        result.push_back("/");
        start = 1;
        end = str.find(delimiter, start);  // Chercher le prochain '/'
    }
    while (end != std::string::npos){
        result.push_back(str.substr(start, end - start));  // Ajouter la partie avant '/'
        result.push_back("/");  // Ajouter le token '/' après chaque mot
        start = end + 1;
        end = str.find(delimiter, start);
    }
        // Ajouter la dernière partie après le dernier '/' si ya
    if (start < str.length()) {
        result.push_back(str.substr(start));  // Ajouter la partie apres le dernier '/'
    }
    return result;
}


Route *Request::findLocation(){
    std::string copyRequest;

    this->_copyRequest = this->_request;
    //0. Decouper la route en token
    std::vector<std::string> rootToken = doSplit(this->_request, '/');
    std::map<std::string, Route *> tmp = this->_server->getConfig()->getRoutes();
    std::map<std::string, Route*>::iterator it;
    it = tmp.begin();
    while (this->_copyRequest != "/"){
        std::cout << "copyRequest -> " << this->_copyRequest << std::endl;
        for (it = tmp.begin(); it != tmp.end(); ++it){
            if (it->first == this->_copyRequest){
                std::cout << "(1) la route dans find location est:" << this->_request << std::endl;
                std::cout << "(1) save request est:" << this->_saveRequest << std::endl;
                if (it->second == NULL)
                    std::cout << "EXISTE PAS" << std::endl;
                return (it->second);
            }
        }
        //2. Règle plus générique
        std::string tmp;
        for (size_t i = 0; i < rootToken.size() - 1; ++i) {
            tmp += rootToken[i];
        }
        std::cout << "Je viens ici" << std::endl;
        //mettre a jour le vector
        this->_copyRequest = tmp;
        this->_saveRequest.insert(0, rootToken[rootToken.size() - 1]);
        rootToken.pop_back();
    }
    if (it == tmp.end())
        it = tmp.find("/");
    std::cout << "(2) la route dans find location est:" << this->_copyRequest << std::endl;
    std::cout << "(2) save request est:" << this->_saveRequest << std::endl;
    std::cout << "EXISTE PAS" << std::endl;
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


void    sendResponse(std::string htmlContent, int fdClient){

    ssize_t content_length = htmlContent.size();
    std::string content_string = toString(content_length);
    std::string response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: " + content_string + "\r\n"
    "\r\n"
    + htmlContent;

    ssize_t bytes_sent = send(fdClient, response.c_str(), response.size(), 0);
    if (bytes_sent < 0) {
        perror("send");
        close(fdClient);
    }
}

void    Request::handleRequest(){
    std::string line;
    std::string htmlContent;
    std::istringstream stream(this->_buffer);
    Route *ptr = NULL;

    std::cout << "REQUEST COMPLETE :" << std::endl;
    std::cout << this->_buffer << std::endl;
    if (std::getline(stream, line)){
        // if (line.find("favicon.ico") == std::string::npos){
            this->getInfoRequest(line);
            if (!this->_request.empty()){
                if ((ptr = this->findLocation()) != NULL)
                {
                    this->checkRequest(ptr);
                    if (!this->_htmlContent.empty())
                        sendResponse(this->_htmlContent, this->_client->getFd());
                    else
                        std::cout << "Aucun contenu ne peut etre affiché!" << std::endl;
                }
                else
                {
                    // SEND ERROR PAGE
                    return ;
                }
            }
        // }
        // else
        // {
        //     if (this->_request == "/favicon.ico") {
        //         std::ifstream file("favicon.ico", std::ios::binary);
        //         if (file) {
        //             std::ostringstream ss;
        //             ss << file.rdbuf();
        //             std::string icon_data = ss.str();
                    
        //             response_headers = "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\nContent-Length: " + 
        //                                std::to_string(icon_data.size()) + "\r\n\r\n";
        //             response_body = icon_data;
        //         } else {
        //             response_headers = "HTTP/1.1 404 Not Found\r\n\r\n";
        //             response_body = "Favicon not found.";
        //         }
        //     }
        // }
    }
}