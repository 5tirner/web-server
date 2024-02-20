#include "../include/Server.hpp"

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

std::string Server::mapUriToFilePath(std::string& uri, location& routeConfig)
{
    std::string filePath;
    std::map<std::string, std::string>::iterator rootIt = routeConfig.root.find("root");
    if (rootIt != routeConfig.root.end())
    {
        filePath = rootIt->second;
        filePath += "/";
    }
    else
        filePath = "/var/www/html/"; // Default path
    if (uri == "/")
    {
        std::map<std::string, std::string>::iterator indexIt = routeConfig.index.find("index");
        if (indexIt != routeConfig.index.end() && !indexIt->second.empty())
        {
            std::istringstream iss(indexIt->second);
            std::string indexFile;
            while (std::getline(iss, indexFile, ' '))
            {
                std::string fullFilePath = filePath + "/" + indexFile;
                if (fileExists(fullFilePath))
                    return fullFilePath; // Return the first existing file
            }
            }
            return filePath + "/index.html"; // Default index file
    }
    else
    {
        std::map<std::string, std::string>::iterator pos  = routeConfig.directory.find("location");
        std::cout << "index: " << pos->second << std::endl;
        size_t start = pos->second.length();
        filePath += uri.substr(start);
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
            if (uri.compare(0, locPath.length(), locPath) == 0)
                return loc; // Found a matching location
        }
    }
    throw std::runtime_error("Route not found for URI: " + uri);
}


void Server::handleRequestGET(int clientSocket, HTTPRequest& request, informations& serverConfig)
{

    location routeConfig = findRouteConfig(request.uri, serverConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("GET") == std::string::npos)
    {    
        sendErrorResponse(clientSocket, 404, "Method Not allowed");
        return;
    }
    // Determine the file path based on the route configuration
    std::string filePath2 = mapUriToFilePath(request.uri, routeConfig);

    std::string filePath = filePath2;
    // Check if the path is a directory
    if (isDirectory(filePath))
    {
        std::vector<location>::iterator it = serverConfig.locationsInfo.begin();
        std::string check = request.uri + it->index["index"];
        if (isRegularFile(check))
        {
            std::string response = "HTTP/1.1 301 OK\r\n";
            response += "Location: " + check + " \r\n";
            response += "\r\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        std::map<std::string, std::string>::iterator autoindexIt = routeConfig.autoindex.find("autoindex");
        if (autoindexIt != routeConfig.autoindex.end() && autoindexIt->second == "on")
        {
            std::string directoryContent = generateDirectoryListing(filePath);
            std::cout << directoryContent << std::endl;
            std::string response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + to_string(directoryContent.size()) + "\r\n";
            response += "\r\n";
            response += directoryContent;
            send(clientSocket, response.c_str(), response.size(), 0);
         }
    }
    else
    {
        // Existing file handling code
        if (!fileExists(filePath))
        {
            std::cout << "wa9ila dkhal l 404\n";
            sendErrorResponse(clientSocket, 404, "Not Found");
            return;
        }
        std::string fileContent = readFileContent(filePath);
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + getMimeType(filePath) + "\r\n";
        response += "Content-Length: " + to_string(fileContent.size()) + "\r\n";
        response += "\r\n";
        response += fileContent;
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}