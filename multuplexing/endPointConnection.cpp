#include "../include/mainHeader.hpp"
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <netdb.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <string>
#include <vector>

connection::connection(void)
{
    
}

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

// void    addrinfoIssue(long status)
// {
//     if (status == EAI_ADDRFAMILY)
//         std::cerr << "EAI_ADDFAM: network host does not have any network" << std::endl;
//     else if (status == EAI_AGAIN)
//         std::cerr << "EAI_AGAIN: server returned a temporary" << std::endl;
//     else if (status == EAI_BADFLAGS)
//         std::cerr << "EAI_BADF: hints.ai_flags  contains  invalid  flags" << std::endl;
//     else if (status == EAI_FAIL)
//         std::cerr << "EAI_FAIL: name server returned a permanent failure" << std::endl;
//     else if (status == EAI_FAMILY)
//         std::cerr << "EAI_FAMILY: The requested address family is not supported" << std::endl;
//     else if (status == EAI_MEMORY)
//         std::cerr << "EAI_MEM: Out of memory" << std::endl;
//     else if (status == EAI_NODATA)
//         std::cerr << "EAI_NODATA: not have any network addresses defined" << std::endl;
//     else if (status == EAI_NONAME)
//         std::cerr << "EAI_NONAME: node  or service is not known" << std::endl;
//     else if (status == EAI_SOCKTYPE)
//         std::cerr << "EAI_SOCKTYPE: requested socket type is not supported" << std::endl;
//     else if (status == EAI_SERVICE)
//         std::cerr << "EAI_SERVICE: The  requested service is not available" << std::endl;
//     else if (status == EAI_SYSTEM)
//         std::cerr << "EAI_SYSTEM: Other system error" << std::endl;
// }

void    connection::serversEndPoint(std::map<int, informations> &info)
{
    std::map<int, informations>::iterator   it = info.begin();
    std::vector<std::vector<std::string> >  checkDupHost;
    std::vector<std::string>                checkTheSameServer;
    checkTheSameServer.reserve(info.size());
    checkDupHost.reserve(info.size());
    while (it != info.end())
    {
        std::cout << "Server Come With Number: " << it->first << std::endl;
        std::vector<std::string> tmp;
        tmp.reserve(2);
        tmp.push_back(it->second.port.at("listen")), tmp.push_back(it->second.host.at("host"));
        if (checkDupHost.size())
        {
            if (std::find(checkDupHost.begin(), checkDupHost.end(), tmp) != checkDupHost.end())
            {
                if (std::find(checkTheSameServer.begin(),
                checkTheSameServer.end(), it->second.serverName.at("server_name")) != checkTheSameServer.end())
                {
                    std::cerr << "The Same Host And ServerName Appears More Than Once:" << std::endl
                    << "The Port: " + tmp[0] + ", The HostIP: " + tmp[1]
                    << ", ServerName: " + it->second.serverName.at("server_name") << std::endl;
                    throw std::runtime_error("Bad Server");
                }
                std::cerr << "The Same Host Appears More Than Once:" << std::endl
                << "The Port: " + tmp[0] + ", The HostIP: " + tmp[1] << std::endl;
                this->notBindingServers[it->second.serverName.at("server_name")] = it->second;
                it++; continue;
            }
        }
        checkDupHost.push_back(tmp), checkTheSameServer.push_back(it->second.serverName.at("server_name"));
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            errorGenerator("Failed To Create Endpoint", -1);
            it++; continue;
        }
        socklen_t   optval = 1;
        struct sockaddr_in sockInfo;
        sockInfo.sin_port = htons(atoi(it->second.port.at("listen").c_str())),
        sockInfo.sin_family = AF_INET,
        sockInfo.sin_addr.s_addr = inet_addr(it->second.host.at("host").c_str());
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
        this->serversSock[fd] = sockInfo; this->OverLoad[fd] = it->second;
        std::cout << "Socket Ready To Listening For The Port: "
        << it->second.port.at("listen") << " With Number: " << fd << std::endl;
        it++;
    }
    std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
    std::map<std::string, informations>::iterator NotBind = this->notBindingServers.begin();
    std::cout << "Those Servers Does not Binding Cause There Host Is Already Binding" << std::endl;
    int R = 1;
    while (NotBind != this->notBindingServers.end())
    {
        std::cout << "Server Number: " << R << std::endl;
        showInfo(NotBind->second);
        R++;
        NotBind++;
    }
}

void    initializeMonitor(struct pollfd &monitor, int fd)
{
    monitor.fd = fd;
    monitor.events = POLLIN | POLLOUT;
}

void connection::processingClientRequest( int rc, char* buffer, Request& rq, const informations& serverInfo )
{
    if ( rq.fetchHeaderDone == false )
        fetchRequestHeader( rq, buffer, rc );
    if ( rq.fetchHeaderDone == true && rq.processingHeaderDone == false )
        processingHeader( rq );
    if ( rq.processingHeaderDone == true )
        processingBody( rq, buffer, rc, serverInfo );
}

void    connection::checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it, const std::map<int, informations>& infoMap) //!yachaab edit here: add localisation vector
{
    if ((monitor.revents & POLLIN))
    {
        std::cout << "Cleint-Side, An Event Happen Into " << monitor.fd << " Endpoint." << std::endl;
        std::cout << "This Client Is Rlated With Server Endpoint " << it->second << std::endl;
        char buffer[2048];
        int rd = read(monitor.fd, buffer, 2047);
        if (rd == -1)
        {
			Logger::log() << "[ Error ] Failed To Read From " << monitor.fd << " Endpoint." << std::endl;
            dropClient(monitor.fd, it);
        }
        else if (rd == 0)
        {
			Logger::log() << "[ Warn ] Client close connection" << std::endl;
			dropClient( monitor.fd, it );
		}
        else if (rd)
        {
            buffer[rd] = '\0';
            /*-------------- yachaab code start ---------------*/
            try {
                try
                {
                    this->Requests.at(monitor.fd);
                }
                catch(...)
                {
                    this->Requests[monitor.fd] = Request();
                }
                std::cout << "===>: " << buffer << std::endl;
                processingClientRequest( rd, buffer, this->Requests.at(monitor.fd),
                infoMap.at( it->second ) );
            }
            catch ( ... )
            {
                std::cerr << codeMsg.statMsg.at(this->Requests[monitor.fd].stat) << std::endl;
                this->Requests.at(monitor.fd).readyToSendRes = true;
            }
            /*-------------- yachaab code end -----------------*/
        }
    }
    else if (monitor.revents & POLLHUP)
    {
        std::cerr << "Error: Client-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
        dropClient(monitor.fd, it);
    }
    else if (monitor.revents & POLLERR)
    {
        std::cerr << "Error: Client-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
        dropClient(monitor.fd, it);
    }
    try
    {
        if ((monitor.revents & POLLOUT) && this->Requests.at(monitor.fd).readyToSendRes)
        {
            try {

                if (!this->Requests.at(monitor.fd).storeHeader)
                {
                    // std::cout << "READY YO SEND RESPONSE: 2" << std::endl;
                    // if (this->Requests.at(monitor.fd).headers.at("method"))
                    if (this->Requests.at(monitor.fd).headers.at("method") == "get")
                        handleRequestGET(monitor.fd, this->Requests.at(monitor.fd), infoMap.at(it->second));
                    else if (this->Requests.at(monitor.fd).headers.at("method") == "delete")
                        handleRequestDELETE(monitor.fd, this->Requests.at(monitor.fd), infoMap.at(it->second));
                }
                /*-------------- yachaab code start -----------------*/
                if ( this->Requests.at(monitor.fd).headers["method"] == "post" )
                {
                    std::string response = creatTemplate( "./src/page.html", this->Requests.at(monitor.fd).stat, codeMsg );
                    sendResponse( monitor.fd, response );
                    Response.at(monitor.fd).status = response::Complete;
                    std::cout << "POST RESPONSE SENT" << std::endl;
                }
                /*-------------- yachaab code ended -----------------*/
                else
                    sendResponseChunk(monitor.fd, Response.at(monitor.fd));
                if (Response.at(monitor.fd).status == response::Complete)
                    throw std::exception();
            } 
            catch (...)
            {
                dropClient(monitor.fd, it);
            }
        }
    }
    catch(...)
    {}
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
            std::cerr << "Clients Those Already Here :" << std::endl;
            for (std::map<int, Request>::iterator it = this->Requests.begin(); it != this->Requests.end(); it++)
                std::cerr << "Clinet Of Fd Number: " << it->first << std::endl;
            std::cout << "New Client Added To Endpoint " << monitor.fd << " With Number " << newClient << '.' << std::endl;
            this->clientsSock[newClient] = monitor.fd;
            this->Response[newClient] = response();
        }
        std::cout << "Number Of Client Now Is: " << this->clientsSock.size() << std::endl;
    }
    else if (monitor.revents & POLLHUP)
        std::cerr << "Error: Server-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
    else if (monitor.revents & POLLERR)
        std::cerr << "Error: Server-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
}

connection::connection(std::map<int, informations> &configData)
{
    this->serversEndPoint(configData);
    // std::cout << "---------------Read This Before Start------------------" << std::endl;
    // std::map<int, informations>::iterator o = this->OverLoad.begin();
    // int i = 1;
    // while (o != this->OverLoad.end())
    // {
    //     std::cout << "Socket For Server Number " << i << " is "
    //     << o->first << '.' << std::endl;
    //     std::cout << o->second.port.at("listen") << std::endl;
    //     std::cout << o->second.host.at("host") << std::endl;
    //     std::cout << o->second.serverName.at("server_name") << std::endl;
    //     o++;
    //     i++;
    //     std::cout << "\\\\" << std::endl;
    // }
    // std::cout << "----------------------------------------------------------" << std::endl;
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
        int eventChecker = poll(monitor, this->clientsSock.size() + this->serversSock.size(), -1);
        if (eventChecker == -1)
            std::cerr << "Error: Poll Failed To When It's Looking For An Event." << std::endl;
        else if (eventChecker)
        {
            i = 0;
            it1 = this->clientsSock.begin();
            while (it1 != this->clientsSock.end())
            {
                this->checkClient(monitor[i], it1, OverLoad);
                it1++;
                i++;
            }
            for (size_t k = 0; k < this->exited.size(); k++)
                    this->clientsSock.erase(this->exited[k]);
            for (size_t k = 0; k < this->requestEnd.size(); k++)
                    this->Requests.erase(this->requestEnd[k]);
            while (!this->EndFd.empty())
            {
                close(this->EndFd.back());
                this->EndFd.pop_back();
            }            
            for (size_t k = 0; k < this->responsetEnd.size(); k++)
                    this->Response.erase(this->responsetEnd[k]);
            this->exited.clear(), this->requestEnd.clear(); this->responsetEnd.clear();
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
/*-------------- yachaab edit start ---------------*/
void connection::dropClient( int& fd, std::map<int, int>::iterator &it )
{
    if (std::find(this->EndFd.begin(), this->EndFd.end(), fd) != this->EndFd.end())
        return;
    this->EndFd.push_back(fd);
    std::cout << "-> Fd Closed." << std::endl;
    std::cout << "Search For Data..." << std::endl;

    std::map<int, Request>::iterator toRemove = this->Requests.find(it->first);
    std::map<int, response>::iterator rm = this->Response.find(it->first);

    if (rm != this->Response.end())
        this->responsetEnd.push_back(it->first);
    if (toRemove != this->Requests.end())
    {
        std::cout << "Found Some Data For: " << toRemove->first << std::endl;
        this->requestEnd.push_back(toRemove);
        std::cout << "Data Deleted." << std::endl;
    }
    std::cerr << "Client " << it->first << " Related With Server "
    << it->second << " Exited." << std::endl;
    this->exited.push_back(it);
    std::cout << "Number Of Client Left: " << this->clientsSock.size() - 1 << std::endl;
}
/*-------------- yachaab edit start ---------------*/