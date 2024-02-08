
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

#define MAX_CLIENTS 30
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
    public:
        Server();
        int createSocet();
        void bindSocket(int port, std::string& ip);
        void listenToSocket();
        void handleConnections();
};

void log(const std::string& message)
{
    std::cout << message << std::endl;
}

void exitWithError(const std::string& errorMessage)
{
    log("ERROR: " + errorMessage);
    exit(1);
}

Server::Server()
{
    bzero(&client_socket, MAX_CLIENTS);
}

int Server::createSocet()
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        exitWithError("Failed to create Socket");
    return sockfd;
}

void Server::bindSocket(int port, std::string& ip)
{
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (bind(sockfd, (sockaddr *)&serverAddr, serverAddrLen) < 0)
        exitWithError("Failed To bind Socket with ip and Port");
}

void Server::listenToSocket()
{
    if (listen(sockfd, 10) < 0)
        exitWithError("Failed to listen to Socket");
}

void Server::handleConnections()
{
    fd_set read_fds;
    struct timeval tv;
    int max_sd;

    while(true)
    {
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        max_sd = sockfd;
    }
}

