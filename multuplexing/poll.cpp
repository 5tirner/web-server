#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sys/poll.h>
#include <map>

int createServerEndpoit(int *server, struct sockaddr_in *sockInfo)
{
    *server = socket(AF_INET, SOCK_STREAM, 0);
    if (*server == -1) return (printf("socket Failed\n"));
    sockInfo->sin_addr.s_addr = 0, sockInfo->sin_port = htons(9090), sockInfo->sin_family = AF_INET;
    socklen_t a = 1;
    int allocateBuffer = setsockopt(*server, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
    if (allocateBuffer == -1) return (printf("setsockopt Failed\n"));
    int assigningName = bind(*server, (struct sockaddr*)sockInfo, sizeof(*sockInfo));
    if (assigningName == -1) return (printf("bind Failed\n"));
    int passiveIt = listen(*server, INT32_MAX);
    if (passiveIt == -1) return (printf("listen Failed\n"));
    return (0);
}

struct  pollfd  fillMonitor(int fd)
{
    struct pollfd toFill;
    toFill.fd = fd, toFill.events = POLLIN;
    return (toFill);
}

int    getClientsReq(std::map<int, std::string> &clientsRequests, int fd, std::vector<int>&clientFds, int index)
{
    char buffer[2048];
    int checker = read(fd, buffer, 2048);
    if (checker == -1)
    {
        std::cerr << "Can't Read From Client Number: " << fd << " Endpoint" << std::endl;  
        return (1);
    }
    else if (checker == 0)
    {
        close(fd);
        std::cout << "Connection Closed By Client Number: " << fd << std::endl;
        clientFds.erase(clientFds.begin() + index);
        std::map<int, std::string>::iterator it = clientsRequests.begin();
        while (it != clientsRequests.end())
        {
            if (it->first == fd)
                break;
            it++;
        }
        clientsRequests.erase(it);
    }
    else
    {
        buffer[checker] = '\0';
        try
        {
            clientsRequests[fd] = clientsRequests.at(fd) + buffer;
        }
        catch (...)
        {
            clientsRequests[fd] = buffer;
        }
        std::map<int, std::string>::iterator it = clientsRequests.begin();
        while (it != clientsRequests.end())
        {
            std::cout << "Client Number: " << it->first << " - It's Content `" + it->second + "`" << std::endl;
            it++;
        }
    }
    return (0);
}

void    watchClient(struct pollfd &monitor, std::vector<int> &clientFds,
        int index, std::map<int, std::string> &clientsReq)
{
    if (monitor.revents & POLLIN)
    {
        std::cout << "Some Data Come From Client Number: " << monitor.fd << std::endl;
        if (getClientsReq(clientsReq, monitor.fd, clientFds, index))
            return ;
        std::cout << "-> Number Of Clinets Now: " << clientFds.size() << std::endl;
    }
    else if (monitor.revents & POLLHUP)
    {
        std::cout << "Client Number " << clientFds[index] << " Has Been Closed" << std::endl;
        clientFds.erase(clientFds.begin() + index);
    }
    else if (monitor.revents & POLLERR)
    {
        std::cout << "There Is An Error In Clinet Number" << monitor.fd << std::endl;
        close(monitor.fd);
        clientFds.erase(clientFds.begin() + index);
    }
}

void    watchServers(int server, struct sockaddr_in sockInfo, struct pollfd monitor,
                    std::vector<int>&servFds, std::vector<int>&clientFds, int index)
{
    if ((monitor.revents & POLLIN))
    {
        socklen_t a = 1;
        int client = accept(server, (struct sockaddr*)&sockInfo, &a);
        if (client == -1)
        {
            std::cerr << "accept Failed" << std::endl;
            return;
        }
        clientFds.push_back(client);
        std::cout << "Clinet NUmber " << client << " Added Successfully" << std::endl;
    }
    else if (monitor.revents & POLLHUP)
    {
        std::cout << "Server Number " << servFds[index] << " Has Been Closed" << std::endl;
        servFds.erase(servFds.begin() + index);
    }
    else if (monitor.revents & POLLERR)
    {
        std::cout << "There Is An Error In Server NUmber " << monitor.fd << std::endl;
        close(monitor.fd);
        servFds.erase(servFds.begin() + index);
    }
}

int main()
{
    int    server;
    struct sockaddr_in sockInfo;
    if (createServerEndpoit(&server, &sockInfo) != 0)
        return (1);
    std::cout << "waiting For Clients.." << std::endl;
    std::vector<int> servFds;
    std::vector<int> clientFds;
    std::map<int, std::string> clinetsReq;
    servFds.push_back(server);
    while (1)
    {
        struct pollfd monitor[servFds.size() + clientFds.size()];
        size_t i = 0, j = 0;
        for (; i < clientFds.size(); i++)
            monitor[i] = fillMonitor(clientFds[i]);
        for (; j < servFds.size(); j++)
        {
            monitor[i] = fillMonitor(servFds[j]);
            i++;
        }
        int checker = (poll(monitor, (servFds.size() + clientFds.size()), 1000));
        if (checker == -1)
            return (printf("poll Failed\n"));
        else if (checker == 0)
            continue;
        else
        {
            i = 0, j = 0;
            for (; i < clientFds.size(); i++)
                watchClient(monitor[i], clientFds, i, clinetsReq);
            for (; j < servFds.size(); j++)
            {
                watchServers(server, sockInfo, monitor[i], servFds, clientFds, j);
                i++;
            }
        }
    }
    close(server);
}