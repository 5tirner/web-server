#include "../include/Server.hpp"

Server::Server()
{
    bzero(&client_socket, MAX_CLIENTS);
}

Server::Server(informations config) : serverConfig(config) {
    bzero(&client_socket, MAX_CLIENTS);
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

int Server::createSocket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        exitWithError("Failed to Create Socket");
    return sockfd;
}

void Server::bindSocket(int port, const std::string& ip)
{
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    int on = 1;
    int seter = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (seter < 0)
            perror("setsockopt(SO_REUSEADDR) failed");
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        exitWithError("Failed to bind Socket");
}

void Server::listenToSocket()
{
    if (listen(sockfd, 3) < 0)
        exitWithError("Failed to Listen To Socket");
}

std::string Server::readFileContent(const std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    std::cout << filePath << std::endl;
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
std::string Server::getMimeType(std::string& filePath)
{
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".mp4"] = "video/mp4";
    size_t dotPos = filePath.rfind('.');
    if (dotPos != std::string::npos)
    {
        std::string extension = filePath.substr(dotPos);
        if (mimeTypes.find(extension) != mimeTypes.end())
            return mimeTypes[extension];
    }
    return "text/plain";
}

void Server::sendErrorResponse(int clientSocket, int errorCode,const std::string& errorMessage)
{
    std::string response = "HTTP/1.1 " + std::to_string(errorCode) + " " + errorMessage + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + std::to_string(errorMessage.size()) + "\r\n";
    response += "\r\n";
    response += errorMessage;
    send(clientSocket, response.c_str(), response.size(), 0);
}

std::string Server::mapUriToFilePath(std::string& uri, location& routeConfig)
{
    std::string filePath;
    std::cout << "uri: " << uri << std::endl;
    std::map<std::string, std::string>::iterator rootIt = routeConfig.root.find("root");
    if (rootIt != routeConfig.root.end())
    {
        filePath = rootIt->second;
        std::cout<<"dkhal hna: " << rootIt->second << std::endl;
    }
    else
        filePath = "/var/www/html"; // Default path

    if (uri == "/")
    {
        std::cout << "slash:\n";
        std::map<std::string, std::string>::iterator indexIt = routeConfig.index.find("index");
        if (indexIt != routeConfig.index.end() && !indexIt->second.empty())
        {
            std::cout << "indexIt->second: " << indexIt->second << std::endl;
            filePath += "/" + indexIt->second;
        }
        else
        {
            filePath += "/index.html"; // Default index file
            std::cout << "FILE: " << filePath << std::endl;
        }
    }
    else
    {
        filePath += uri;
        std::cout << "+uri: " << filePath << std::endl;
    }
    return filePath;
}

location Server::findRouteConfig(std::string& uri,  informations& serverConfig)
{
    for (size_t i = 0; i < serverConfig.locationsInfo.size(); ++i)
    {
        location& loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            std::string& locPath = it->second;
            std::cout << "localpath: " << locPath << std::endl;
            if (uri.compare(0, locPath.length(), locPath) == 0)
            {
                std::cout << "second: " << loc.index.size() << std::endl;
                return loc; // Found a matching location
            }
        }
    }
    throw std::runtime_error("Route not found for URI: " + uri);
}

void Server::handleRequestGET(int clientSocket,  HTTPRequest& request,  informations& serverConfig)
{
    // Determine the correct route based on the request URI
    printf("dokali\n");
    location routeConfig = findRouteConfig(request.uri, serverConfig);
    printf("dokali1\n");
    // Determine the file path based on the route configuration
    std::string filePath2 = mapUriToFilePath(request.uri, routeConfig);

    std::cout << "filepath: " << filePath2 << std::endl;
    std::string filePath = "." + filePath2;
    if (!fileExists(filePath))
    {
        sendErrorResponse(clientSocket, 404, "Not Found");
        return ;
    }
    std::cout << "start read file: \n";
    std::cout << "after add . :" << filePath << std::endl;
    std::string fileContent = readFileContent(filePath);

    std::cout << "+++++++++++++++++++++++++: "<< std::endl;

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + getMimeType(filePath) + "\r\n";
    response += "Content-Length: " + std::to_string(fileContent.size()) + "\r\n";
    response += "\r\n";
    response += fileContent;
    send(clientSocket, response.c_str(), response.size(), 0);
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

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_socket[i];
            if (sd > 0)
                FD_SET(sd, &read_fds);
            if (sd > max_sd)
                max_sd = sd;
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int activity = select(max_sd + 1, &read_fds, NULL, NULL, &tv);
        if ((activity < 0) && (errno != EINTR))
            exitWithError("Select error");

        if (FD_ISSET(sockfd, &read_fds))
        {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int new_socket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
            if (new_socket < 0) exitWithError("Accept failed");
            fcntl(new_socket, F_SETFL, O_NONBLOCK);
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    requests[new_socket] = HTTPRequest(); // Initialize a new HTTPRequest for this socket
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_socket[i];
            if (FD_ISSET(sd, &read_fds))
            {
                char buffer[1024] = {0};
                int valread = read(sd, buffer, sizeof(buffer));
                if (valread > 0)
                {
                    requests[sd].appendData(buffer, valread);
                    if (requests[sd].isComplete())
                    {
                        try
                        {
                            requests[sd].parse(requests[sd].rawRequest);
                            std::cout << "request: " << requests[sd].getFullRequest() << std::endl;
                            // for (const auto& pair : requests[sd].headers)
                            // {
                            //     std::cout << "Key: " << pair.first;
                            //     std::cout << " Values: ";
                            //     for (const auto& value : pair.second)
                            //         std::cout << " " << value;
                            //     std::cout << std::endl;
                            // }
                            if (requests[sd].method == "GET")
                                handleRequestGET(sd ,requests[sd], serverConfig);
                            if (requests[sd].method == "POST")
                                handleRequestPOST(sd ,requests[sd]);
                            std::string response = "HTTP/1.1 200 OK\r\n";
                            response += "Content-Type: \r\n";
                            response += "Content-Length: 10\r\n";
                            response += "\r\nHello From Server";
                            send(sd, response.c_str(), response.size(), 0);
                            requests[sd].clear(); // Clear the request for future use
                        }
                        catch (std::runtime_error& e)
                        {
                            std::cout << e.what() << std::endl;
                        }
                    }
                }
                else if (valread == 0 || (valread < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
                {
                    close(sd);
                    client_socket[i] = 0;
                    requests.erase(sd); // Remove the request from map
                }
            }
        }
    }
}

