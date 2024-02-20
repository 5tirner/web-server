#include "../include/Server.hpp"

Server::Server()
{
    memset(client_socket, 0, sizeof(client_socket));
}

Server::Server(int exitSocket, informations &config , std::string& port, std::string& host)
: sockfd(exitSocket)
{
    this->port = port;
    this->host = host;
    this->serverConfig = config;
    memset(client_socket, 0, sizeof(client_socket));
}


void Server::setConfig(const informations& config)
{
    serverConfig = config;
}
void log(const std::string& message)
{
    std::cout << message << std::endl;
}

void exitWithError(const std::string& errorMessage)
{
    log("ERROR: " + errorMessage);
    exit(1);
}

void setNonBlocking(int sock)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl F_GETFL");
		exit(EXIT_FAILURE);
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl F_SETFL , O_NONBLCOK");
		exit(EXIT_FAILURE);
	}
}

int createAndBindSocket(const std::string& port, const std::string& host)
{
    int sockfd;
    struct sockaddr_in serverAddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    // Set socket to non-blocking mode
    setNonBlocking(sockfd);
    // Enable SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("ERROR setting SO_REUSEADDR");
        close(sockfd);
        exit(1);
    }
    // Bind socket
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
    serverAddr.sin_port = htons(atoi(port.c_str()));
    if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("ERROR on binding");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}


void Server::listenToSocket()
{
    if (listen(sockfd, SOMAXCONN) < 0)
    {
        perror("ERROR on listen");
        exit(1);
    }
}

std::string Server::readFileContent(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    // std::cout << filePath << std::endl;
    if (!file)
        exitWithError("Error: Unable to open file.");
    std::string content;
    std::string line;
    while (std::getline(file, line))
        content += line + "\n";

    file.close();
    return content;
}

bool Server::fileExists(std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    return file.good();
}

std::string to_string(int value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

void Server::sendErrorResponse(int clientSocket, int errorCode,const std::string& errorMessage)
{
    std::string response = "HTTP/1.1 " + to_string(errorCode) + " " + errorMessage + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + to_string(errorMessage.size()) + "\r\n";
    response += "\r\n";
    response += errorMessage;
    send(clientSocket, response.c_str(), response.size(), 0);
}

void Server::initializeEpoll()
{
    epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    struct epoll_event ev;
    ev.events = EPOLLIN; // We're interested in read events
    ev.data.fd = sockfd; // sockfd is your server's listening socket

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
}

void Server::run()
{
    signal(SIGPIPE, SIG_IGN);
    struct epoll_event ev, events[MAX_EVENTS];

        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; n++)
        {
            int current_fd = events[n].data.fd;
            std::cout << "events: " << events[n].data.fd << std::endl;
            if (current_fd == sockfd)
            {
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                int new_socket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (new_socket < 0)
                {
                    perror("accept");
                    continue;
                }
                setNonBlocking(new_socket);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1)
                {
                    perror("epoll_ctl: new_socket");
                    close(new_socket);
                    exit(1);
                }
                client_socket[new_socket] = new_socket;
                std::cout << "new clientfd: " << new_socket << std::endl;
                requests[new_socket] = HTTPRequest();
            }
            else
            {
                char buffer[1024];
                int valread = read(current_fd, buffer, 1024);
                if (valread > 0)
                {
                    requests[current_fd].appendData(buffer, valread);
                    std::cout << requests[current_fd].rawRequest << std::endl;
                    if (requests[current_fd].isComplete())
                    {
                        try
                        {
                            std::cout << "9bal awldi dkhal l if\n";
                            requests[current_fd].parse(requests[current_fd].rawRequest);
                                std::cout << "dkhal l if\n";
                                if (requests[current_fd].method == "GET")
                                    handleRequestGET(current_fd, requests[current_fd], serverConfig);
                                else if (requests[current_fd].method == "POST")
                                    handleRequestPOST(current_fd, requests[current_fd]);
                                else if (requests[current_fd].method == "DELETE")
                                    handleRequestDELETE(current_fd, requests[current_fd], serverConfig);
                                else
                                    sendErrorResponse(current_fd, 501, "Not Implemented");
                                requests[current_fd].clear();
                        }
                        catch (std::runtime_error& e)
                        {
                            sendErrorResponse(current_fd, 400, e.what());
                        }
                    }
                }
                else
                {
                    std::cout << "the client has been deconected\n";
                        close(current_fd);
                        client_socket[n] = -1;
                        requests.erase(current_fd);
                }
            }
        }
}




