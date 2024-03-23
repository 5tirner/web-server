#include "../include/mainHeader.hpp"
#include <fcntl.h>
#include <fstream>
#include <stdexcept>

std::string getMimeType(std::string& filePath)
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

bool fileExists( std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool hasReadPermission(const std::string& path) {
    return (access(path.c_str(), R_OK) == 0);
}


std::string mapUriToFilePath( std::string& uri,  location& locConfig)
{
    try
    {
        std::string filePath = locConfig.root.at("root"); // Use .at() for const map
        std::string locPath = locConfig.directory.at("location");
        std::cout << "===================> : " << locPath << " " << filePath << std::endl;
        std::string pathSuffix;
        std::string fullPath;
        // if (uri.find(locPath) == 0)
        // {
            std::cout << "----------->DKhal l IF lWALA: " << uri << std::endl;
            pathSuffix = uri.substr(locPath.length());
        // }
        // else
        // {
        //     // if (isDirectory(uri))
        //     std::cout << "----------->DKhal l IF TANYA: " << uri << std::endl;
        //     pathSuffix = uri;
        // }
            std::cout << "---------->: " << pathSuffix << std::endl;
        if (pathSuffix.empty() || pathSuffix[pathSuffix.size() - 1] == '/')
        {
            std::istringstream iss(locConfig.index.at("index")); // Use .at() here as well
            std::string indexFile;
            while (std::getline(iss, indexFile, ' '))
            {
                fullPath = filePath + (pathSuffix[pathSuffix.length() - 1] == '/' ? pathSuffix : pathSuffix + "/") + indexFile;
                if (fileExists(fullPath))
                    return fullPath; // Found an index file, return its path
            }
            std::cout << "--->: " << fullPath << std::endl;
            return filePath;
        }
    else
    {
        // If the pathSuffix is not empty and does not end with '/', directly append it to filePath.
        if (filePath[filePath.length() -1 ] != '/')
            filePath += "/";
        filePath += pathSuffix;
        std::cout << "--->======+>: " << filePath << std::endl;
        if (fileExists(filePath))
            return filePath;
        std::cout << "--->: " << fullPath << std::endl;
        return filePath;
        // Handle file not found if necessary.
    }
    }
    catch (const std::out_of_range& e)
    {
        // Handle the case where a key does not exist in the map
        std::cerr << "Key not found in configuration: " << e.what() << '\n';
        // Handle error, possibly return a default value or error indicator
    }
    return ""; // Placeholder return to satisfy all control paths
}


location findRouteConfig(std::string& uri,const informations& serverConfig)
{
    location loc;
    for (size_t i = serverConfig.locationsInfo.size(); i > 0;)
    {
        i--;
        loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::const_iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            const std::string& locPath = it->second;
            std::cout << "--->1: " << locPath << std::endl;
            std::cout << "=====>: " << uri << std::endl;
            if (uri.compare(0, locPath.length(), locPath) == 0)
            {
                std::cout << "--->2: " << locPath << std::endl;
                return loc; // Found a matching location
            }
        }
    }
    throw std::runtime_error("kkk");
    // loc = serverConfig.locationsInfo[0];
    // loc.root["root"] = serverConfig.defaultRoot.at("default_root");
    // return loc;
}


bool isRegularFile(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISREG(statbuf.st_mode);
}

bool isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}
std::string generateDirectoryListing(const std::string& path)
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

std::string to_string(int value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

void connection::handleRequestGET(int clientSocket, Request& request,const informations& serverConfig)
{
    location routeConfig;
    try {
         routeConfig = findRouteConfig(request.headers["uri"], serverConfig);
    
    } catch (...) {
        serveErrorPage(clientSocket, 404, serverConfig); // File not found
    
    }
    if (routeConfig.allowed_methodes["allowed_methodes"].find("GET") == std::string::npos)
    {
        serveErrorPage(clientSocket, 405, serverConfig);
        return;
    }
    std::map<std::string, std::string>::iterator it = routeConfig.Return.find("return");
    if ( it != routeConfig.Return.end() && !it->second.empty())
    {
        std::string redirectURL = it->second; // URL to redirect to
        std::cout << "==========================>: RedirectURL: " << redirectURL << std::endl;
        std::string responseD = "HTTP/1.1 301 Moved Permanently\r\n";
        responseD += "Location: " + redirectURL + "\r\n";
        responseD += "Content-Length: 0\r\n";
        responseD += "Connection: close\r\n\r\n";
        response responseData;
        responseData.setResponseHeader(responseD);
        request.storeHeader = true;
        Response[clientSocket] = responseData;
        return;
    }
    else
    {
        std::map<std::string, std::string>::const_iterator it = routeConfig.directory.find("location");
        std::cout << "Location test: ++++++++========>: " << it->second << std::endl;
        std::string filePath = mapUriToFilePath(request.headers["uri"], routeConfig);
        // if (routeConfig.cgi.at("cgi") == "on")
        // {
        //     //work on cgi now you can use anything you want ba3bab3a3bab3abb3abab3aba3b
        // }

            // Check if the file exists and has read permission
    // if (!fileExists(filePath)) {
    //     serveErrorPage(clientSocket, 404, serverConfig); // File not found
    //     return;
    // }
    // if (!hasReadPermission(filePath)) {
    //     serveErrorPage(clientSocket, 403, serverConfig); // Forbidden
    //     return;
    // }
        if (!access(filePath.c_str(), F_OK))
        {
            if (access(filePath.c_str(), R_OK))
            {
                serveErrorPage(clientSocket, 403, serverConfig);
                return;
            }
        }
        else
        {
            serveErrorPage(clientSocket, 404, serverConfig); // Not Found
            return;
        }
        // if (!fileExists(filePath))
        // {
        //     std::cout << "===========================================><><><>\n";
        //         serveErrorPage(clientSocket, 404, serverConfig);
        //         return;
        // }
        std::cout << "=======>: path: " << filePath << std::endl;
        std::string responseD;
        if (isDirectory(filePath))
        {
            std::vector<location>::const_iterator it = serverConfig.locationsInfo.begin();
            std::string check = request.headers["uri"] + it->index.at("index");
            std::map<std::string, std::string>::iterator autoindexIt = routeConfig.autoindex.find("autoindex");
            if (isRegularFile(check))
            {
                responseD = "HTTP/1.1 301 OK\r\n";
                responseD += "Location: " + check + " \r\n";
            }
            else if (autoindexIt != routeConfig.autoindex.end() && autoindexIt->second == "on")
            {
                std::string directoryContent = generateDirectoryListing(filePath);
                std::cout << directoryContent << std::endl;
                responseD = "HTTP/1.1 200 OK\r\n";
                responseD += "Content-Type: text/html\r\n";
                responseD += "Content-Length: " + to_string(directoryContent.size()) + "\r\n";
                
                responseD += "\r\n";
                responseD += directoryContent;
            }
        }
        else
        {
            responseD = "HTTP/1.1 200 OK\r\n";
            responseD += "Content-Type: " + getMimeType(filePath) + "\r\n";
        }
        response responseData;
        responseData.filePath = filePath;
        responseData.setResponseHeader(responseD);
        request.storeHeader = true;
        Response[clientSocket] = responseData;

    }
}