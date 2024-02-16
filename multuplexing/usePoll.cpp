#include <asm-generic/socket.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <vector>

int main()
{
    int socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServer == -1) return (printf("Socket Failed\n"));
    std::cout << "Socket Created." << std::endl;
    struct sockaddr_in describeSockAddres;
    describeSockAddres.sin_family = AF_INET, describeSockAddres.sin_port =  htons(7000);
    std::cout << "Conevert From Host Byte Order To Network Bytes Order Done." << std::endl;
    int optval = 1;
    int allocateBuffer = setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (allocateBuffer == -1) return (printf("setsocket Failed\n"));
    std::cout << "Buffer Allocated." << std::endl;
    int sockName = bind(socketServer, (struct sockaddr *)&describeSockAddres, sizeof(describeSockAddres));
    if (sockName == -1) return (printf("bind Failed\n"));
    std::cout << "Socket Get Name." << std::endl;
    int bePassive = listen(socketServer, INT32_MAX);
    if (bePassive == -1) return (printf("Listen Failed\n"));
    std::cout << "Socket Is Passive Now And Waiting For Clients..." << std::endl;
    std::vector<int>poeple;
    while (1)
    {
        struct pollfd *monitor = new struct pollfd[poeple.size() + 1];
        monitor->events = POLLIN;
        monitor->fd = socketServer;
        socklen_t fill = sizeof(describeSockAddres);
        int socketClient = accept(socketServer, (struct sockaddr*)&describeSockAddres, &fill);
        if (socketClient == -1) return (printf("accept Failed\n"));
        char buffer[1024];
        while (fill)
        {
            fill = read(socketClient, buffer, 1024);
            if (fill == -1)
                return (printf("Can't Read From Client Endpoint"));
            else if (fill > 0)
            {
                std::cout << "Read " << fill << " From Client Endpoint Here Are These:" << std::endl;
                write(1, buffer, fill);
            }
        }
        close(socketClient);
    }
    std::cout << "Connection Closed By Client, BYE!" << std::endl;
    close(socketServer);
}