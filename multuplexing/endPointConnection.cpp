#include "../include/mainHeader.hpp"
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

connection::connection(void){}

connection::connection(const connection &other){*this = other;}

connection &connection::operator=(const connection &other)
{
    (void)other;
    return (*this);
}

connection::~connection(void){}

void errorGenerator(std::string err, int fd)
{
    if (fd != -1)
        close(fd);
    std::cerr << err << std::endl;
}

void    connection::serversEndPoint(std::map<int, informations> &info)
{
    std::map<int, informations>::iterator it = info.begin();
    while (it != info.end())
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            errorGenerator("Failed To Create Endpoint", -1);
            it++;
            continue;
        }
        socklen_t   optval = 1;
        struct sockaddr_in sockInfo;
        sockInfo.sin_port = htons(atoi(it->second.port.at("listen").c_str())),
                                    sockInfo.sin_family = AF_INET, sockInfo.sin_addr.s_addr = 0;
        int bufferAllocation = setsockopt(fd, SOL_SOCKET,
                                            SO_REUSEADDR, &optval, sizeof(optval));
        if (bufferAllocation == -1)
        {
            errorGenerator("Can't Allocate Buffer For A Socket", fd);
            it++;
            continue;
        }
        int AssignName = bind(fd, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
        if (AssignName == -1)
        {
            errorGenerator("Socket Can't Get A Name To Be Defined On The Network", fd);
            it++;
            continue;
        }
        int listening = listen(fd, 2147483647);
        if (listening == -1)
        {
            errorGenerator("Can't Turn The Socket To Passive One", fd);
            it++;
            continue;
        }
        this->serversSock[fd] = sockInfo;
        std::cout << "Socket Raedy To Listening For The Port: " << it->second.port.at("listen") << " With Number: " << fd << std::endl;
        it++;
    }
}

void    connection::checkForEvents(struct pollfd monitor[])
{
    int fdsNbr = poll(monitor, this->serversSock.size() + this->clientsSock.size(), 100);
    if (fdsNbr == -1)
    {
        std::cerr << "Poll Failed When Tried To Looking For An event" << std::endl;
        return ;
    }
    else if (fdsNbr != 0)
    {
        size_t i = 0;
        std::vector<int>::iterator it1 = this->clientsSock.begin();
        while (it1 != this->clientsSock.end())
        {
            if (monitor[i].revents & POLLHUP)
            {
                std::cerr << "Error: The Connection Of The Client Number " << *it1 << std::endl;
                this->clientsSock.erase(it1);
            }
            else if (monitor[i].revents & POLLERR)
            {
                std::cerr << "Error: Client Number " << *it1 << " Is Not Good" << std::endl;
                close(*it1);
                this->clientsSock.erase(it1);
            }
            else
            {
                char buffer[2048];
                std::cout << "Some Data Coming From Client Number " << *it1 << std::endl;
                int rd = read(*it1, buffer, 2048);
                if (rd == -1)
                {
                    std::cerr << "Error: Can't Raed From Client Number " << *it1 << std::endl;
                    close(*it1);
                    this->clientsSock.erase(it1);
                }
                else if (rd == 0)
                {
                    std::cout << "Clinet Number " << *it1 << " Closed The Connection" << std::endl;
                    close(*it1);
                    this->clientsSock.erase(it1);
                    std::cout << this->clientsSock.size() << " Client Left" << std::endl;
                }
                else
                {
                    buffer[rd] = 0;
                    try
                    {
                        this->clientsReq[*it1] = clientsReq.at(*it1) + buffer;
                    }
                    catch(...)
                    {
                        this->clientsReq[*it1] = buffer;
                    }
                }
            }
            i++;
        }
        std::map<int, struct sockaddr_in>::iterator it = this->serversSock.begin();
        while (it != this->serversSock.end())
        {
            if (monitor[i].revents & POLLHUP)
            {
                std::cerr << "Server " << monitor[i].fd << " Connection Closed " << std::endl;
                this->serversSock.erase(it);
            }
            else if (monitor[i].revents & POLLERR)
            {
                std::cerr << "Error: Server Number " << it->first << " Is Not Good" << std::endl;
                close(it->first);
                this->serversSock.erase(it);
            }
            else if ((monitor[i].revents & POLLIN) || (monitor[i].revents & POLLOUT))
            {
                socklen_t a = 1;
                int newClient = accept(it->first, (struct sockaddr*)&it->second, &a);
                if (newClient == -1)
                    std::cerr << "Error: Accept Failed To Get New Client Endpoint For Server: " << it->first << std::endl;
                else
                {
                    std::cout << "Client Number " << newClient << " Now Is Connected With The Server Number " << it->first << std::endl;
                    this->clientsSock.push_back(newClient);
                }
            }
            it++;
            i++;
        }
    }
}

connection::connection(std::map<int, informations> &configData)
{
    this->serversEndPoint(configData);
    while (1)
    {
        struct pollfd monitor[this->serversSock.size() + this->clientsSock.size()];
        std::map<int, struct sockaddr_in>::iterator it = this->serversSock.begin();
        size_t i = 0, j = 0;
        while (j < this->clientsSock.size())
        {
            monitor[i].events = POLLIN | POLLOUT;
            monitor[i].fd = this->clientsSock[j];
            i++;
            j++;
        }
        while (it != this->serversSock.end())
        {
            monitor[i].events = POLLIN | POLLOUT;
            monitor[i].fd = it->first; 
            it++;
            i++;
        }
        checkForEvents(monitor);
        //break;
    }
}

