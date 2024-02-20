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
#include<set>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <dirent.h>
#include <sstream>
#include<csignal>
#include<cstdlib>

#include "mainHeader.hpp"
#include "HTTPRequest.hpp"


#define MAX_CLIENTS 10000


class Server
{
    private:
        std::string serverName;
        int sockfd;
        int client_socket[MAX_CLIENTS];
        std::map<int, HTTPRequest> requests;
        informations serverConfig;
        int epoll_fd;
        static const int MAX_EVENTS = 10;
        std::string      port;
        std::string      host;
    public:
        int numberServers;
        void initializeEpoll();
        void initializeServer(); // New method to initialize server
        void setupEpoll();
        void run();
        bool isRegularFile(const std::string& path);
        Server();
        Server(int exitSocket,informations &config ,std::string& port, std::string& host);
        void createSocket();
        void bindSocket();
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
        void acceptNewConnection();
        void handleExistingConnections(fd_set& read_fds);
        void initializeFileDescriptorSet(fd_set& read_fds, int& max_sd);
        void processRequest(int clientSocket, HTTPRequest& request);
        bool isDirectory(const std::string& path);
        bool isRequestForThisServer(HTTPRequest& request, std::string& serverName);
        std::string generateDirectoryListing(const std::string& path);
        void handleCGIRequest(int clientSocket, HTTPRequest& request, informations& serverConfig);
        std::string getScriptPathFromURI(const std::string& uri);
};

int createAndBindSocket(const std::string& port, const std::string& host);
std::string to_string(int value);


#endif