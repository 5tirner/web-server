#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <fcntl.h>

#include "mainHeader.hpp"

#define MAX_CLIENTS 30


struct FormDataPart
{
    std::map<std::string, std::string> headers;
    std::string body;
};

struct MultipartFormData
{
    std::vector<FormDataPart> parts;
};

typedef std::map<std::string, std::string> JsonData;

class HTTPRequest
{
    public:
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::map<std::string, std::vector<std::string> > headers;
        std::string body;
        std::map<std::string, std::string> queryParams;
        std::string rawRequest;
        void parse(std::string& rawRequest);
        void clear();
        bool isComplete();
        void appendData(const char* buffer, int length);
        HTTPRequest();

    std::string getFullRequest() const;
};


class Server
{
    private:
        int sockfd;
        int client_socket[MAX_CLIENTS];
        std::map<int, HTTPRequest> requests;
        informations serverConfig;
    public:
        Server();
        Server(informations config);
        int createSocket();
        void bindSocket(int port, const std::string& ip);
        void listenToSocket();
        void handleConnections();
        std::string readFileContent(const std::string& filePath);
        void handleRequestGET( int clientSocket,  HTTPRequest& request,  informations& serverConfig);
        std::string getMimeType(std::string& filePath);
        void handleRequestPOST(int clientSocket,  HTTPRequest& request);   
        void handleRequestDELETE(int clientSocket,  HTTPRequest& request,  informations& serverConfig);  
        bool fileExists(std::string& filePath);
        std::string mapUriToFilePath( std::string& uri, location& routeConfig);
        location findRouteConfig(std::string& uri, informations& serverConfig);
        void sendErrorResponse(int clientSocket, int errorCode,const std::string& errorMessage);
        void setConfig(const informations& config);

};



#endif