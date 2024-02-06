#include "../include/server.hpp"

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

