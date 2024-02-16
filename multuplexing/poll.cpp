#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sys/poll.h>
int main()
{
    std::string p;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) return (printf("socket Failed\n"));
    struct sockaddr_in  sockInfo;
    sockInfo.sin_addr.s_addr = 0, sockInfo.sin_port = htons(9090), sockInfo.sin_family = AF_INET;
    socklen_t a = 1;
    int allocateBuffer = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
    if (allocateBuffer == -1) return (printf("setsockopt Failed\n"));
    int assigningName = bind(server, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
    if (assigningName == -1) return (printf("bind Failed\n"));
    int passiveIt = listen(server, INT32_MAX);
    if (passiveIt == -1) return (printf("listen Failed\n"));
    std::cout << "waiting For Clients" << std::endl;
    std::vector<int> servFds;
    std::vector<int> clientFds;
    servFds.push_back(server);
    while (1)
    {
        struct pollfd monitor[servFds.size() + clientFds.size()];
        size_t i = 0;
        for (; i < clientFds.size(); i++)
        {
            struct pollfd toFill;
            toFill.fd = clientFds[i];
            toFill.events = POLLIN | POLLOUT;
            monitor[i] = toFill;
        }
        for (; i < servFds.size(); i++)
        {
            struct pollfd toFill;
            toFill.fd = servFds[i];
            toFill.events = POLLIN;
            monitor[i] = toFill;
        }
        int checker = (poll(monitor, (servFds.size() + clientFds.size()), 1000));
        if (checker == -1)
            return (printf("poll Failed\n"));
        else if (checker == 0) continue;
            //std::cout << "1000ms Pass Without Any Event" << std::endl;
        else
        {
            for (size_t i = 0; i < clientFds.size(); i++)
            {
                if ((monitor[i].revents & POLLIN))
                {
                    std::cout << "Some Data Come From Client Number: " << monitor[i].fd << std::endl;
                    std::cout << "-> Number Of Clinets Now: " << clientFds.size() << std::endl;
                    for (size_t i = 0; i < clientFds.size(); i++)
                        std::cout << "C.Number: " << clientFds[i] << std::endl;
                }
                else if (monitor[i].revents & POLLHUP)
                {
                    std::cout << "Client Number " << monitor[i].fd << " Has Been Closed" << std::endl;
                    clientFds.erase(clientFds.begin() + i);
                }
                else if (monitor[i].revents & POLLERR)
                {
                    std::cout << "There Is An Error In Clinet Number" << monitor[i].fd << std::endl;
                    close(monitor[i].fd);
                    clientFds.erase(clientFds.begin() + i);
                }
            }
            for (size_t i = 0; i < servFds.size(); i++)
            {
                if ((monitor[i].revents & POLLIN))
                {
                    int client = accept(server, (struct sockaddr*)&sockInfo, &a);
                    if (client == -1) return (printf("accept Failed\n"));
                    clientFds.push_back(client);
                    std::cout << "Clinet NUmber " << client << " Added Successfully" << std::endl;
                    std::cout << "-> Number Of Servers Now: " << servFds.size() << std::endl;
                    for (size_t i = 0; i < servFds.size(); i++)
                        std::cout << "S.Number: " << servFds[i] << std::endl;
                }
                else if (monitor[i].revents & POLLHUP)
                {
                    std::cout << "Server Number " << monitor[i].fd << " Has Been Closed" << std::endl;
                    servFds.erase(servFds.begin() + i);
                }
                else if (monitor[i].revents & POLLERR)
                {
                    std::cout << "There Is An Error In Server NUmber " << monitor[i].fd << std::endl;
                    close(monitor[i].fd);
                    servFds.erase(servFds.begin() + i);
                }
            }
        }
    }
    close(server);
}