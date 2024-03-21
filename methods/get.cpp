#include "../include/mainHeader.hpp"
#include <fcntl.h>
#include <fstream>

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

std::string mapUriToFilePath( std::string& uri,  location& locConfig)
{
    try
    {
        std::string rootPath = locConfig.root.at("root"); // Use .at() for const map
        std::string filePath = rootPath; // Start constructing the file path from the root
        std::string locPath = locConfig.directory.at("location");
        std::string pathSuffix;
        std::string fullPath;
        if (uri.find(locPath) == 0)
            pathSuffix = uri.substr(locPath.length());
        else
            pathSuffix = uri;
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
            return fullPath;
            // Optional: Handle case when no index file is found...
        }
    else
    {
        // If the pathSuffix is not empty and does not end with '/', directly append it to filePath.
        if (filePath[filePath.length() -1 ] != '/')
            filePath += "/";
        filePath += pathSuffix;
        if (fileExists(filePath))
            return filePath;
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
            std::cout << "===========: " << locPath << std::endl;
            if (uri.compare(0, locPath.length(), locPath) == 0)
            {
                std::cout << "===============>>><<<<<<>>>>: " << loc.directory.at("location") << std::endl;
                return loc;
            }
        }
    }
    location loc = serverConfig.locationsInfo[0];
    return loc;
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
    location routeConfig = findRouteConfig(request.headers["uri"], serverConfig);
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
        std::string filePath2 = mapUriToFilePath(request.headers["uri"], routeConfig);
        std::cout << "===========>: " << filePath2 << std::endl;
        std::string filePath = filePath2;
        // if (routeConfig.cgi.at("cgi") == "on")
        // {
        //     //work on cgi now you can use anything you want ba3bab3a3bab3abb3abab3aba3b
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
            // std::cout << "check if this correct" << std::endl;;
            /* -------------- yachaab code start ----------------- */
            // std::fstream file;
            // file.open( "./media/video/morpho.mp4", std::fstream::binary | std::fstream::ate | std::fstream::in );
            // size_t size = file.tellg();
            // file.close();
            // // std::cout << "size: " << size << std::endl;
            // responseD += "Content-Length: " + to_string( size ) + "\r\n";
            // std::cout << "MIME TYPE: " << getMimeType(filePath) << std::endl;
            // std::cout << "content length: " << xnxx << std::endl;
            /*-------------- yachaab code ended -----------------*/
        }
        // if (request.flagRespons == 0)
        // {
            response responseData;

            responseData.filePath = filePath;
            responseData.setResponseHeader(responseD);
            request.storeHeader = true;
            Response[clientSocket] = responseData;

    }
}