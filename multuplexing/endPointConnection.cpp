#include "../include/mainHeader.hpp"
#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
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
            it++; continue;
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
            it++; continue;
        }
        int AssignName = bind(fd, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
        if (AssignName == -1)
        {
            errorGenerator("Socket Can't Get A Name To Be Defined On The Network", fd);
            it++; continue;
        }
        int listening = listen(fd, 2147483647);
        if (listening == -1)
        {
            errorGenerator("Can't Turn The Socket To Passive One", fd);
            it++; continue;
        }
        this->serversSock[fd] = sockInfo;
        std::cout << "Socket Ready To Listening For The Port: " << it->second.port.at("listen") << " With Number: " << fd << std::endl;
        it++;
    }
}

void    initializeMonitor(struct pollfd &monitor, int fd)
{
    monitor.fd = fd;
    monitor.events = POLLIN | POLLOUT;
}

void    connection::checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it)
{
    if ((monitor.revents & POLLIN))
    {
        std::cout << "Cleint-Side, An Event Happen Into " << monitor.fd << " Endpoint." << std::endl;
        std::cout << "This Client Is Rlated With Server Endpoint " << it->second << std::endl;
        char buffer[2048];
        int rd = read(monitor.fd, buffer, 2047);
        if (rd == -1)
        {
            std::cerr << "Error: Failed To Read From " << monitor.fd << " Endpoint." << std::endl;
            close(monitor.fd);
            this->clientsSock.erase(it);
        }
        else if (rd == 0)
        {
            std::cout << "Warrning: Connection Closed From Client " << monitor.fd << '.' << std::endl;
            close(monitor.fd);
            std::cout << "-> Fd Closed." << std::endl;
            std::cout << "Search For Data..." << std::endl;
            std::map<int, Request>::iterator toRemove = this->Requests.find(it->first);
            // while (toRemove != this->Requests.end())
            // {
            //     if (toRemove->first == it->first)
            //         break ;
            //     it++;
            // }
            if (toRemove != this->Requests.end())
            {
                std::cout << "Found Some Data For: " << toRemove->first << std::endl;
                this->Requests.erase(toRemove);
                std::cout << "Data Deleted." << std::endl;
            }
            //this->clientsSock.erase(it);
            //this->exited.push_back(it);
            std::cout << "Number Of Client Left: " << this->clientsSock.size() - 1 << std::endl;
        }
        else if (rd)
        {
            buffer[rd] = '\0';
            /*-------------- yachaab code start ---------------*/
            try {
                if ( this->Requests.find(monitor.fd) == this->Requests.end() )
                    this->Requests[monitor.fd] = clientRequest();
                if (this->Requests[monitor.fd].fetchHeaderDone == false)
                    fetchRequestHeader( this->Requests[monitor.fd], buffer );
                if ( this->Requests[monitor.fd].fetchHeaderDone == true && this->Requests[monitor.fd].processingHeaderDone == false )
                    processingHeader( this->Requests[monitor.fd] );
            } catch (...) {
                // do somthis in case of error mostly drop client and check code status
            }
            
            /*-------------- yachaab code end -----------------*/
        }
    }
    else if (monitor.revents & POLLHUP)
    {
        std::cerr << "Error: Client-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
        this->clientsSock.erase(it);
    }
    else if (monitor.revents & POLLERR)
    {
        std::cerr << "Error: Client-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
        close(monitor.fd);
        this->clientsSock.erase(it); 
    }
}

void    connection::checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it)
{
    if ((monitor.revents & POLLIN))
    {
        std::cout << "Server-Side, An event Comming Into " << monitor.fd << " Endpoint." << std::endl;
        socklen_t   addLen = sizeof(it->second);
        int newClient = accept(monitor.fd, (struct sockaddr *)&it->second, &addLen);
        if (newClient == -1)
            std::cerr << "Error: Filed To Create New EndPoint With Socket " << monitor.fd << std::endl;
        else
        {
            std::cout << "New Client Added To Endpoint " << monitor.fd << " With Number " << newClient << '.' << std::endl;
            this->clientsSock[newClient] = monitor.fd;
        }
    }
    else if (monitor.revents & POLLHUP)
        std::cerr << "Error: Server-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
    else if (monitor.revents & POLLERR)
        std::cerr << "Error: Server-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
}

connection::connection(std::map<int, informations> &configData)
{
    this->serversEndPoint(configData);
    // std::cout << "Servers Endpoint:" << std::endl; 
    // while (it != this->serversSock.end())
    // {
    //     std::cout << "- Number: " << it->first << " -> It's Struct Info: "
    //     << it->second.sin_family << "-" << it->second.sin_port << "-" << it->second.sin_addr.s_addr << std::endl;
    //     it++;
    // }
    while (1)
    {
        struct pollfd monitor[this->serversSock.size() + this->clientsSock.size()];
        std::map<int, int>::iterator it1 = this->clientsSock.begin();
        std::map<int, struct sockaddr_in>::iterator it = this->serversSock.begin();
        size_t i = 0;
        while (it1 != this->clientsSock.end())
        {
            initializeMonitor(monitor[i], it1->first);
            it1++;
            i++;
        }
        while (it != this->serversSock.end())
        {
            initializeMonitor(monitor[i], it->first);
            it++;
            i++;
        }
        int eventChecker = poll(monitor, this->clientsSock.size() + this->serversSock.size(), 100);
        if (eventChecker == -1)
            std::cerr << "Error: Poll Failed To When It's Looking For An Event." << std::endl;
        else if (eventChecker)
        {
            i = 0;
            it1 = this->clientsSock.begin();
            while (it1 != this->clientsSock.end())
            {
                this->checkClient(monitor[i], it1);
                it1++;
                i++;
            }
            it = this->serversSock.begin();
            while (it != this->serversSock.end())
            {
                checkServer(monitor[i], it);
                it++;
                i++;
            }
        }
    }
}

