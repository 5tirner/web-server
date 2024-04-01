#include "../include/mainHeader.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>


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

bool fileExists(std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    return file.good();
}
bool isPathWithinRoot(std::string& resolvedPath, std::string& rootPath)
{
    std::cerr << "res: --->: " << resolvedPath << std::endl;
    std::cerr << "rooot: --->: " << rootPath << std::endl;
    if (resolvedPath[resolvedPath.length() - 1] != '/')
        resolvedPath += "/";
    if (resolvedPath.substr(0, rootPath.size()) == rootPath)
        return true;
    return false;
}

std::string resolveFilePath(std::string& path)
{
    char *realPath = NULL;
    realPath = realpath(path.c_str(), realPath);
    
    if (realPath == NULL)
        return "";
    return std::string(realPath);
}

std::string mapUriToFilePath( std::string& uri,  location locConfig)
{
    try
    {
        std::string rootPath = locConfig.root.at("root"); // Use .at() for const map

        std::cerr << "================>: rootPath: " << rootPath << std::endl;
        std::string filePath = rootPath; // Start constructing the file path from the root
        std::string locPath = locConfig.directory.at("location");
        std::cerr << "================>: locPath: " << locPath << std::endl;
        std::string pathSuffix;
        if (uri.find(locPath) == 0)
            pathSuffix = uri.substr(locPath.length());
        else
            pathSuffix = uri;
        if ((pathSuffix.empty() || pathSuffix[pathSuffix.size() - 1] == '/') && locConfig.autoindex.at("autoindex") != "on")
        {
            std::istringstream iss(locConfig.index.at("index")); // Use .at() here as well
            std::string indexFile;
            while (std::getline(iss, indexFile, ' '))
            {
                std::string fullPath = filePath + (pathSuffix[pathSuffix.length() - 1] == '/' ? pathSuffix : pathSuffix + "/") + indexFile;
                std::string tmp = fullPath;
                tmp = resolveFilePath(rootPath);
                std::cerr << "tmp: " << tmp.length() << std::endl;
                if (tmp.empty())
                    return "dkhal";
                if (!isPathWithinRoot(fullPath, rootPath))
                {
                    std::cerr << "\n\n---->\n\n";
                    return "dkhal";
                }

                return fullPath; // Found an index file, return its path
            }
            // Optional: Handle case when no index file is found...
        }
        else
        {
            // If the pathSuffix is not empty and does not end with '/', directly append it to filePath.
            if (filePath[filePath.length() -1 ] != '/')
                filePath += "/";
            filePath += pathSuffix;
            std::string tmp = filePath;
            tmp = resolveFilePath(rootPath);
            std::cerr << "tmp: " << tmp.length() << std::endl;
            if (tmp.empty())
                return "dkhal";
            if (!isPathWithinRoot(filePath, rootPath))
            {
                std::cerr << "\n\n---->\n\n";
                return "dkhal";
            }
            // if (fileExists(filePath))
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
    for (size_t i = 0; i < serverConfig.locationsInfo.size(); ++i)
    {
        location loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::const_iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            std::string locPath = it->second;
            if (uri.compare(0, locPath.length(), locPath) == 0)
                return loc;
        }
    }
    throw std::runtime_error("no location found");
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
    std::cerr << "path dire: " << path << std::endl;
    html << "<html><head><title>Index of " << path << "</title></head><body>";
    html << "<h1>Index of " << path << "</h1><hr><pre>";

    dir = opendir(path.c_str());
    if (dir != NULL)
    {
        std::cerr << "dir: " << dir << std::endl;
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
    try
    {
        routeConfig = findRouteConfig(request.headers["uri"], serverConfig);
    }
    catch(...)
    {
        serveErrorPage(clientSocket, 404, serverConfig);
        return;
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
        size_t spacePos = redirectURL.find(' ');
        if (spacePos != std::string::npos)
            redirectURL = redirectURL.substr(spacePos + 1);
        if (redirectURL.find("http://") != 0 && redirectURL.find("https://") != 0)
            redirectURL = "http://" + redirectURL;
        std::string responseD = "HTTP/1.1 301 Moved Permanently\r\n";
        responseD += "Location: " + redirectURL + "\r\n";
        responseD += "Content-Lengt+h: 0\r\n";
        responseD += "Connection: close\r\n\r\n";
        response responseData;
        responseData.setResponseHeader(responseD);
        request.storeHeader = true;
        Response[clientSocket] = responseData;
        return;
    }
    else
    {
        std::string filePath2 = mapUriToFilePath(request.headers["uri"], routeConfig);
        std::cerr << "===========>->: " << filePath2 << std::endl;
        std::string filePath = filePath2;
        if (filePath == "dkhal")
        {
            std::cerr << "\n\n---->\n\n";
            serveErrorPage(clientSocket, 403, serverConfig);
            return;
        }
        std::cerr << "fillllePath: " << filePath << std::endl;
        if (filePath[filePath.length() - 1] == '/')
            filePath = filePath.substr(0, filePath.length() - 1);
        if (!access(filePath.c_str(), F_OK))
        {
            if (access(filePath.c_str(), R_OK))
            {
                std::cerr << "\n\n\n\n=======>\n\n\n\n\n\n";
                serveErrorPage(clientSocket, 403, serverConfig);
                return;
            }
        }
        else
        {
            std::cerr << "--->=====>:\n";
            serveErrorPage(clientSocket, 404, serverConfig);
            return;
        }
        std::string executer = GetExtentions(filePath);
        if (routeConfig.cgi.at("cgi") == "on" && executer != "NormalFile")
        {
            try
            {
                filePath = cgiFile("GET", filePath, executer, filePath2);
            }
            catch(const char *err)
            {
                std::cerr << err << std::endl;
            }
            std::cerr << "The File Geted By CGI Is: " + filePath << std::endl;
        }
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
                std::cerr << directoryContent << std::endl;
                responseD = "HTTP/1.1 200 OK\r\n";
                responseD += "Content-Type: text/html\r\n";
                responseD += "Content-Length: " + to_string(directoryContent.size()) + "\r\n";
                
                responseD += "\r\n";
                responseD += directoryContent;
            }
            else
                serveErrorPage(clientSocket, 404, serverConfig);
        }
        else
        {
            responseD = "HTTP/1.1 200 OK\r\n";
            responseD += "Content-Type: " + getMimeType(filePath) + "\r\n";
            /*-------------- yachaab code ended -----------------*/
        }

        response responseData;

        responseData.filePath = filePath;
        responseData.setResponseHeader(responseD);
        request.storeHeader = true;
        Response[clientSocket] = responseData;

    }
}