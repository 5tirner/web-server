#include "../include/mainHeader.hpp"
#include <asm-generic/socket.h>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

int errorsGenerator(std::string error, int fd)
{
    std::cerr << error << std::endl;
    if (fd != -1)
        close(fd);
    return (1);
}

connection::connection(void){}

connection::connection(const connection &other){*this = other;}

connection &connection::operator=(const connection &other)
{
    this->configData = other.configData;
    return (*this);
}

connection::~connection(void){}

connection::connection(std::map<int, informations> &configData)
{
    (void)configData;
}

int main()
{
    std::cout << "- Try Create Socket..." << std::endl;
    int serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD == -1) return (errorsGenerator("Fail To Creat Endpoint For Server", -1));
    std::cout << "Socket Created Successfully With Number: " << serverFD << '.' << std::endl;
    struct sockaddr_in SI;
    SI.sin_family = AF_INET, SI.sin_port = htons(8800);
    std::cout << "- Try To Allocate Buffer Space To The Socket " << serverFD << "..." << std::endl;
    int optval = 1;
    // int bufferSpace = setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    // if (bufferSpace == -1)
    //     return (errorsGenerator("Can't Aloccate Buffer Space For The Socket", serverFD));
    std::cout << "Buffer Allocated Successfully." << std::endl;
    std::cout << "- Try To Assigning Name To The Socket " << serverFD << "..." << std::endl;
    int socketName = bind(serverFD, (struct sockaddr *)&SI, sizeof(SI));
    if (socketName == -1)
        return (errorsGenerator("Can't Assigning Name To The Socket", serverFD));
    std::cout << "Name Assigned Successfully." << std::endl;
    std::cout << "- Try To Turn Socket " << serverFD << " Into A Passive Socket..." << std::endl; 
    int turnPassive = listen(serverFD, INT32_MAX);
    if (turnPassive == -1)
        return (errorsGenerator("Can't Make THe Socket Passive", serverFD));
    std::cout << "Socket Ready To Listening." << std::endl;
    socklen_t grb = sizeof(SI);
    std::cout << "Waiting For Client..." << std::endl;
    int clientfd = accept(serverFD, (struct sockaddr*)&SI, &grb);
    if (clientfd == -1)
        return (errorsGenerator("Can't Create Endpoint For Client", serverFD));
    std::cout << "Client Is Here With The Number " << clientfd << '.' << std::endl;
    while (grb != 0)
    {
        char buffer[100000];
        grb = read(clientfd, buffer, 100000);
        if (grb == -1)
        {
            close(clientfd);
            return (errorsGenerator("Can't Read From Clinet Endpoint", serverFD));
        }
        else if (grb)
        {
            std::cout << "Reading " << grb << " Bytes From Client Side, Here Are These:" << std::endl;
            write(1, buffer, grb);
            std::cout << std::endl;
        }
        // if (write(clientfd, "Welcome\n", std::strlen("Welcome\n")) == -1)
        // {
        //     close(clientfd);
        //     return (errorsGenerator("Can't Write Into Client Endpoint", serverFD));
        // }
        //sleep(5);
    }
    std::cout << "Connection Closed From Client Side" << std::endl;
    close(clientfd);
    close(serverFD);
}
