#include "../include/Server.hpp"

bool Server::isRegularFile(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISREG(statbuf.st_mode);
}

bool Server::isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

std::string Server::generateDirectoryListing(const std::string& path)
{
    DIR *dir;
    struct dirent *ent;
    std::ostringstream html;
    std::cout << "path dire: " << path << std::endl;
    html << "<html><head><title>Index of " << path << "</title></head><body>";
    html << "<h1>Index of " << path << "</h1><hr><pre>";

    dir = opendir(path.c_str());
    if (dir != NULL)
    {
        std::cout << "dir: " << dir << std::endl;
        while ((ent = readdir(dir)) != NULL)
            html << "<a href='" << ent->d_name << "'>" << ent->d_name << "</a><br>";
        closedir(dir);
    }
    else
        html << "Cannot access directory.";
    html << "</pre><hr></body></html>";
    return html.str();
}

void Server::handleRequestDELETE(int clientSocket, HTTPRequest& request, informations& serverConfig)
{
    location routeConfig = findRouteConfig(request.uri, serverConfig);

    std::string filePath = mapUriToFilePath(request.uri, routeConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("DELETE") == std::string::npos)
    {
        sendErrorResponse(clientSocket, 405, "Method Not Allowed");
        return;
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);

    // Check if the file/directory exists
    if (access(filePath.c_str(), F_OK) == -1)
    {
        sendErrorResponse(clientSocket, 404, "Not Found");
        return;
    }
    // Check for necessary permissions
    if (access(filePath.c_str(), W_OK) == -1)
    {
        sendErrorResponse(clientSocket, 403, "Forbidden");
        return;
    }
    // Check if it's a directory and handle accordingly
    if (S_ISDIR(path_stat.st_mode))
    {
        sendErrorResponse(clientSocket, 403, "Forbidden: Cannot delete directory");
        return;
    }
    std::cout << "aytremova had sat: " << filePath << std::endl;
    if (std::remove(filePath.c_str()) != 0)
    {
        sendErrorResponse(clientSocket, 500, "Internal Server Error");
        return;
    }
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}