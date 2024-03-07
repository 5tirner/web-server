#include "../include/mainHeader.hpp"

bool removeDirectory(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        return false;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string entryName = entry->d_name;
        if (entryName == "." || entryName == "..")
            continue;

        std::string fullPath = path + "/" + entryName;

        if (isDirectory(fullPath))
        {
            if (!removeDirectory(fullPath))
            {
                closedir(dir);
                return false;
            }
        }
        else
        {
            if (std::remove(fullPath.c_str()) != 0) {
                closedir(dir);
                return false;
            }
        }
    }
    closedir(dir);
    return std::remove(path.c_str()) == 0;
}

void connection::handleRequestDELETE(int clientSocket, Request& request,const informations& serverConfig)
{
    location routeConfig = findRouteConfig(request.headers["uri"], serverConfig);

    std::string filePath = mapUriToFilePath(request.headers["uri"], routeConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("DELETE") == std::string::npos)
    {
        // sendErrorResponse(clientSocket, 405, "Method Not Allowed");
        return;
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);

    // Check if the file/directory exists
    if (access(filePath.c_str(), F_OK) == -1)
    {
        // sendErrorResponse(clientSocket, 404, "Not Found");
        return;
    }
    // Check for necessary permissions
    if (access(filePath.c_str(), W_OK) == -1)
    {
        // sendErrorResponse(clientSocket, 403, "Forbidden");
        return;
    }
    // Check if it's a directory and handle accordingly
    if (S_ISDIR(path_stat.st_mode))
    {
        if (!removeDirectory(filePath))
        {
            // sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    else
    { 
        if (std::remove(filePath.c_str()) != 0)
        {
            // sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}