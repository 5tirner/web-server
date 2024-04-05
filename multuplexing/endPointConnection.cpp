#include "../include/mainHeader.hpp"
#include <stdexcept>

connection::connection(void) {}

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
    std::map<int, informations>::iterator   it = info.begin();
    std::vector<std::vector<std::string> >  checkDupHost;
    std::vector<std::string>                checkTheSameServer;
    checkTheSameServer.reserve(info.size());
    checkDupHost.reserve(info.size());
    while (it != info.end())
    {
        //std::cerr << "Server Number: " << it->first << std::endl;
        std::vector<std::string> tmp;
        tmp.reserve(2);
        tmp.push_back(it->second.port.at("listen")), tmp.push_back(it->second.host.at("host"));
        if (checkDupHost.size())
        {
            if (std::find(checkDupHost.begin(), checkDupHost.end(), tmp) != checkDupHost.end())
            {
                // std::cout << it->second.serverName.at("server_name") + ": " << std::endl
                // << "To Compare With : " << std::endl;
                // for (size_t o = 0; o < checkTheSameServer.size(); o++)
                //     std::cout << "`" + checkTheSameServer[o] + "`" << std::endl;
                if (std::find(checkTheSameServer.begin(),
                checkTheSameServer.end(), it->second.serverName.at("server_name")) != checkTheSameServer.end())
                {
                    std::cerr << "The Same Host And ServerName Appears More Than Once:" << std::endl
                    << "The Port: " + tmp[0] + ", The HostIP: " + tmp[1]
                    << ", ServerName: " + it->second.serverName.at("server_name") 
                    << " In Server Number: " << it->first << std::endl;
                    throw std::runtime_error("Bad Server");
                }
                std::cerr << "The Same Host Appears More Than Once:" << std::endl
                << "The Port: " + tmp[0] + ", The HostIP: " + tmp[1] << " Server NUmber: "
                <<it->first << std::endl;
                std::cerr << "Server Not bound" << std::endl;
                this->notBindingServers[it->second.serverName.at("server_name")] = it->second;
                checkTheSameServer.push_back(it->second.serverName.at("server_name"));
                it++; continue;
            }
        }
        checkTheSameServer.push_back(it->second.serverName.at("server_name"));
        checkDupHost.push_back(tmp);
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
        // std::cerr << " Socket Descriptor: " << fd << " Listen On Port Number: "
        // << it->second.port.at("listen") << std::endl;
        it++;
    }
    // std::cerr << "<------------ Unbinded Servers Start ------------>" << std::endl;
    // std::map<std::string, informations>::iterator NotBind = this->notBindingServers.begin();
    // int R = 1;
    // while (NotBind != this->notBindingServers.end())
    // {
    //     // std::cerr << "Server Number: " << R << std::endl;
    //     showInfo(NotBind->second);
    //     R++;
    //     NotBind++;
    // }
    // std::cerr << "<------------------------ ---------------------->" << std::endl;
}

void    initializeMonitor(struct pollfd &monitor, int fd)
{
    monitor.fd = fd;
    monitor.events = POLLIN | POLLOUT;
}

void connection::processingClientRequest( int rc, char* buffer, Request& rq, int serverID)
{
    if ( rq.fetchHeaderDone == false )
        fetchRequestHeader( rq, buffer, rc );
    if ( rq.fetchHeaderDone == true && rq.processingHeaderDone == false )
        processingHeader( rq );
    if ( rq.processingHeaderDone == true )
        processingBody( rq, buffer, rc, serverID );
}

void    connection::checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it, const std::map<int, informations>& infoMap)
{
    if ((monitor.revents & POLLIN))
    {
        // std::cerr << "Client-Side, An event happend on socket number: " << monitor.fd << " Endpoint." << std::endl;
        // std::cerr << "Client with socket: " << monitor.fd << " is related to server Endpoint: " << it->second << std::endl;
        char buffer[2048];
        this->clientTimerToSendRequest.at(monitor.fd) = clock();
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
            // std::cerr << "buffer: " << buffer << std::endl;
            // std::cerr << "buffer: " << buffer << std::endl;
            /*-------------- yachaab code start ---------------*/
            try {
                try
                {
                    this->Requests.at(monitor.fd);
                }
                catch(...)
                {
                    this->Requests.insert( std::make_pair( monitor.fd, Request() ) );
                }
                processingClientRequest( rd, buffer, this->Requests.at(monitor.fd), it->second );
            }
            catch ( ... )
            {
                // std::cerr << "\033[35mPROCESSING CLIENT REQUEST + BODY DONE WITH STAT:" << this->Requests.at(monitor.fd).stat << "\033[0m" << std::endl;
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
        if ( (monitor.revents & POLLOUT) && this->Requests.at(monitor.fd).readyToSendRes )
        {
            try {

                if (!this->Requests.at(monitor.fd).storeHeader)
                {
                   try
                   {
                        std::string host = this->Requests.at(monitor.fd).headers.at("host");
                        informations info;
                        std::map<std::string, informations>::iterator iter = notBindingServers.find( host );
                        if ( iter != notBindingServers.end() )
                            info = iter->second;
                        else
                            info = infoMap.at(it->second);
                        
                        if (this->Requests.at(monitor.fd).headers.at("method") == "get")
                        {
                            std::cout << "SEND RESPONSE GET" << std::endl;
                            handleRequestGET(monitor.fd, this->Requests.at(monitor.fd), info);
                        }
                        else if (this->Requests.at(monitor.fd).headers.at("method") == "delete")
                        {
                            std::cout << "SEND RESPONSE DELETE" << std::endl;
                            handleRequestDELETE(monitor.fd, this->Requests.at(monitor.fd), info);
                        }
                        else
                        {
                            std::cout << "SEND RESPONSE OTHERS" << std::endl;
                            serveErrorPage(monitor.fd, this->Requests.at(monitor.fd).stat,  info);
                        }
                   }
                   catch ( ... )
                   {
                     std::cout << "SEND RESPONSE UNKOWN" << std::endl;
                     informations info;
                     if ( this->Requests.at(monitor.fd).headers.at("method") == "head" )
                        this->Requests.at(monitor.fd).stat = -1;
                     serveErrorPage(monitor.fd, this->Requests.at(monitor.fd).stat, info );
                   }
                   
                    // if (!this->Requests.at(monitor.fd).cgiGET)
                    //     sendResponseChunk(monitor.fd, Response.at(monitor.fd));
                    // else
                    // {
                    //     int status = Response.at(monitor.fd).sendResponseFromCGI(monitor.fd, 
                    //         this->Cgires.at(monitor.fd), Response.at(monitor.fd));
                    //     if (status == 408 || status == 500)
                    //     {
                    //         serveErrorPage(monitor.fd, status,  Response.at(monitor.fd).info);
                    //         Response.at(monitor.fd).status = response::Complete;
                    //     }
                    // }
                    // // try
                    // {
                    //     try
                    //     {
                    //         std::cout << "Not Binding One: " << std::endl;
                    //         std::string allMethods = "get delete post put patch options"
                    //         , myMethod = this->Requests.at(monitor.fd).headers.at("method");
                    //         std::cout << "The Method Comes Is: " + myMethod << std::endl;
                    //         if (allMethods.find(myMethod) == std::string::npos)
                    //             serveErrorPage(monitor.fd, 400, Response[monitor.fd].info);
                    //         else if (myMethod != "get" && myMethod != "delete" && myMethod != "post")
                    //             serveErrorPage(monitor.fd, 501, Response[monitor.fd].info);
                    //         else
                    //         {
                    //             std::string host = this->Requests.at(monitor.fd).headers.at("host");
                    //             notBindingServers.at(host);
                    //             if (this->Requests.at(monitor.fd).headers.at("method") == "get")
                    //                 handleRequestGET(monitor.fd, this->Requests.at(monitor.fd), notBindingServers.at(host));
                    //             else if (this->Requests.at(monitor.fd).headers.at("method") == "delete")
                    //                 handleRequestDELETE(monitor.fd, this->Requests.at(monitor.fd), notBindingServers.at(host));
                    //             else if (myMethod == "post")
                    //                 serveErrorPage(monitor.fd, this->Requests.at(monitor.fd).stat,  notBindingServers.at(host));
                    //         }
                    //     }
                    //     catch(...)
                    //     {
                    //         std::cout << "Binding One" << std::endl;
                    //         std::string allMethods = "get delete post put patch options"
                    //         , myMethod = this->Requests.at(monitor.fd).headers.at("method");
                    //         std::cout << "Method Comes Is: " << myMethod << std::endl;
                    //         if (allMethods.find(myMethod) == std::string::npos)
                    //             serveErrorPage(monitor.fd, 400, Response[monitor.fd].info);
                    //         else if (myMethod != "get" && myMethod != "delete" && myMethod != "post")
                    //             serveErrorPage(monitor.fd, 501, Response[monitor.fd].info);
                    //         else
                    //         {
                    //             if (myMethod== "get")
                    //                 handleRequestGET(monitor.fd, this->Requests.at(monitor.fd), infoMap.at(it->second));
                    //             else if (myMethod== "delete")
                    //                 handleRequestDELETE(monitor.fd, this->Requests.at(monitor.fd),infoMap.at(it->second));
                    //             else if (myMethod == "post")
                    //                 serveErrorPage(monitor.fd, this->Requests.at(monitor.fd).stat,  infoMap.at(it->second));
                    //         }
                    //     }
                    // }
                    // catch( ... )
                    // {
                    //     std::cout << "SEND RESPONSE 3" << std::endl;
                    //     serveErrorPage(monitor.fd, this->Requests.at(monitor.fd).stat,  infoMap.at(it->second));
                    // }
                }
                /*-------------- yachaab code ended -----------------*/
                else
                {
                    if (!this->Requests.at(monitor.fd).cgiGET)
                        sendResponseChunk(monitor.fd, Response.at(monitor.fd));
                    else
                    {
                        int status = Response.at(monitor.fd).sendResponseFromCGI(monitor.fd, 
                        this->Cgires.at(monitor.fd), Response.at(monitor.fd));
                        if (status == 408 || status == 500)
                        {
                            serveErrorPage(monitor.fd, status,  Response[monitor.fd].info);
                            Response.at(monitor.fd).status = response::Complete;
                        }
                    }
                }
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
        // std::cerr << "Server-Side, An event happend on server socket number: " << monitor.fd << std::endl;
        socklen_t   addLen = sizeof(it->second);
        int newClient = accept(monitor.fd, (struct sockaddr *)&it->second, &addLen);
        if (newClient == -1)
            std::cerr << "Error: Filed To Create New EndPoint With Socket " << monitor.fd << std::endl;
        else
        {
            // std::cerr << "New client number: " << newClient << " Added to server endpoint: " << monitor.fd << std::endl;
            this->clientTimerToSendRequest[newClient] = clock();
            this->clientTimerToEndRequest[newClient] = clock();
            this->clientsSock[newClient] = monitor.fd;
            this->Response[newClient] = response();
            this->Cgires[newClient] = ParsedCGIOutput();
        }
        // std::cerr << "Number Of Client Now Is: " << this->clientsSock.size() << std::endl;
    }
    else if (monitor.revents & POLLHUP)
        std::cerr << "Error: Server-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
    else if (monitor.revents & POLLERR)
        std::cerr << "Error: Server-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
}

connection::connection(std::map<int, informations> &configData)
{
    this->serversEndPoint(configData);
    // std::cerr << "---------------Read This Before Start------------------" << std::endl;
    // std::map<int, informations>::iterator o = this->OverLoad.begin();
    // int i = 1;
    // while (o != this->OverLoad.end())
    // {
    //     std::cerr << "Socket For Server Number " << i << " is "
    //     << o->first << '.' << std::endl;
    //     std::cerr << o->second.port.at("listen") << std::endl;
    //     std::cerr << o->second.host.at("host") << std::endl;
    //     std::cerr << o->second.serverName.at("server_name") << std::endl;
    //     o++;
    //     i++;
    //     std::cerr << "\\\\" << std::endl;
    // }
    // std::cerr << "----------------------------------------------------------" << std::endl;
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
        {
            std::cerr << "Error: Poll Failed When It's Looking For An Event." << std::endl;
            throw std::runtime_error("The Main Function Failed");
        }
        else if (eventChecker)
        {
            i = 0;
            it1 = this->clientsSock.begin();
            while (it1 != this->clientsSock.end())
            {
                this->checkClient(monitor[i], it1, OverLoad);
                if ((clock() - this->clientTimerToSendRequest.at(it1->first)) / CLOCKS_PER_SEC >= 30)
                {
                    std::cerr << "Clinet Of Fd Number: " << it1->first
                    << " Ralated With Server Of Fd Number " << it1->second
                    << " Spend Much Time Without New Request " << std::endl;
                    this->dropClient(monitor[i].fd, it1);
                }
                else if ((clock() - this->clientTimerToEndRequest.at(it1->first)) / CLOCKS_PER_SEC >= 60)
                {
                    std::cerr << "Clinet Of Fd Number: " << it1->first
                    << " Ralated With Server Of Fd Number " << it1->second
                    << " Spend Much Time To End His Request " << std::endl;
                    this->dropClient(monitor[i].fd, it1);
                }
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
    try
    {
        if ( this->Requests.at( fd ).bodyStream != NULL )
        {
            if ( this->Requests.at( fd ).bodyStream->is_open() )
                this->Requests.at( fd ).bodyStream->close();
            delete this->Requests.at( fd ).bodyStream;
            this->Requests.at( fd ).bodyStream = NULL;
        }
    }
    catch(...){}
    if (std::find(this->EndFd.begin(), this->EndFd.end(), fd) != this->EndFd.end())
        return;
    this->EndFd.push_back(fd);
    // std::cerr << "-> Fd Closed." << std::endl;
    // std::cerr << "Search For Data..." << std::endl;
    std::map<int, Request>::iterator toRemove = this->Requests.find(it->first);
    std::map<int, response>::iterator rm = this->Response.find(it->first);
    cleanupResponseFiles(rm->second.removeFiles);
    if (rm != this->Response.end())
        this->responsetEnd.push_back(it->first);
    if (toRemove != this->Requests.end())
    {
        // std::cerr << "Found Some Data For: " << toRemove->first << std::endl;
        this->requestEnd.push_back(toRemove);
        // std::cerr << "Data Deleted." << std::endl;
    }
   // std::cerr << "Client " << it->first << " Related With Server "
    //<< it->second << " Exited." << std::endl;
    this->exited.push_back(it);
    //std::cerr << "Number Of Client Left: " << this->clientsSock.size() - 1 << std::endl;
}
/*-------------- yachaab edit start ---------------*/