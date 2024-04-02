#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>



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
std::string resolveFilePath(std::string& path)
{
    char *realPath = realpath(path.c_str(), NULL);
    std::string resolvedPath;
    if (realPath)
    {
        resolvedPath = std::string(realPath);
        free(realPath); 
    }
    return resolvedPath;
}

bool isPathWithinRoot(std::string& resolvedPath, std::string& rootPath)
{
    if (resolvedPath.find(rootPath) != 0)
        return false;
    return true;
}

std::string mapUriToFilePath( std::string& uri,  location locConfig)
{
    std::string rootPath = locConfig.root.at("root");
    std::string locPath = locConfig.directory.at("location");
    std::string pathSuffix = uri.substr(locPath.length());
    std::string fullPath = rootPath;
    std::string resolvedPath;
    if (pathSuffix.empty() || pathSuffix[0] != '/')
        fullPath += "/";
    fullPath += pathSuffix;
    std::string indexPath;
    if ((pathSuffix.empty() || pathSuffix[pathSuffix.length() - 1] == '/'))
    {
        std::istringstream iss(locConfig.index.at("index"));
        std::string indexFile;
        while (std::getline(iss, indexFile, ' '))
        {
            indexPath = fullPath + indexFile;
            std::cout << "IndexPath: " << indexPath << std::endl; 
            std::string tmp = indexPath;
            tmp = resolveFilePath(rootPath);
            if (tmp.empty())
                throw std::runtime_error("there is a problem");
            if (!isPathWithinRoot(indexPath, rootPath))
            {
                std::cout << "\n\n---->\n\n";
                throw std::runtime_error("there is a problem");
            }
            // resolvedPath = resolveFilePath(indexPath);
            if (!indexPath.empty() && fileExists(indexPath) && isPathWithinRoot(indexPath, rootPath))
                return indexPath;
        }
        if (indexPath.empty() || !isPathWithinRoot(indexPath, rootPath))
            throw std::runtime_error("there is a problem");
    }
    else
    {
        // If the pathSuffix is not empty and does not end with '/', directly append it to filePath.
        // if (fullPath[fullPath.length() -1 ] != '/')
        //     fullPath += "/";
        // fullPath += pathSuffix;
        std::string tmp = fullPath;
        tmp = resolveFilePath(rootPath);
        if (tmp.empty())
            throw std::runtime_error("there is a problem");
        if (!isPathWithinRoot(fullPath, rootPath))
        {
            std::cout << "\n\n---->\n\n";
            throw std::runtime_error("there is a problem");
        }
        // if (fileExists(fullPath))
        return fullPath;
        // Handle file not found if necessary.
    }
    // else if (!pathSuffix.empty())
    // {
    //     resolvedPath = resolveFilePath(fullPath);
    //     std::cout << "===>: resolvePath: " << resolvedPath << std::endl;
    //     if (!resolvedPath.empty() && fileExists(resolvedPath) && isPathWithinRoot(resolvedPath, rootPath))
    //         return resolvedPath;
    // }
    return fullPath;
}

location findRouteConfig(std::string& uri,const informations& serverConfig)
{
    std::string matched;
    location loc_save;
    for (size_t i = 0; i < serverConfig.locationsInfo.size(); ++i)
    {
        location loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::const_iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            std::string locPath = it->second;
            if (uri.compare(0, locPath.length(), locPath) == 0 && locPath.length() > matched.length())
            {
                loc_save = loc;
                matched = locPath;
            }
        }
    }
    if (matched.empty())
        throw std::runtime_error("no location found");
    return loc_save;
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
std::string getStatusMessage(int statusCode)
{
    std::map<int, std::string> statusMessages;
    statusMessages[200] = "OK";
    statusMessages[201] = "Created";
    statusMessages[400] = "Bad Request";
    statusMessages[403] = "Forbidden";
    statusMessages[404] = "Not Found";
    statusMessages[413] = "Payload Too Large";
    statusMessages[500] = "Internal Server Error";
    statusMessages[501] = "Not Implemented";
    statusMessages[503] = "Service Unavailable";
    if (statusMessages.find(statusCode) != statusMessages.end())
        return statusMessages[statusCode];
    else
        return "Status Unknown";
}
static void lowcase( std::string& str )
{
	for ( size_t i = 0; i < str.length(); i++  )
	{
		if ( str[ i ] >= 65 && str[ i ] <= 90 )
			str[ i ] += 32;
	}
}

bool setHeadet(std::string header)
{
    std::vector<std::string> headers;
    headers.push_back("accept-ch");
    headers.push_back("access-control-allow-origin");
    headers.push_back("accept-patch");
    headers.push_back("accept-ranges");
    headers.push_back("age");
    headers.push_back("allow");
    headers.push_back("alt-svc");
    headers.push_back("cache-control");
    headers.push_back("connection");
    headers.push_back("content-disposition");
    headers.push_back("content-encoding");
    headers.push_back("content-language");
    headers.push_back("content-length");
    headers.push_back("content-location");
    headers.push_back("content-md5");
    headers.push_back("content-range");
    headers.push_back("content-type");
    headers.push_back("date");
    headers.push_back("delta-base");
    headers.push_back("etag");
    headers.push_back("expires");
    headers.push_back("im");
    headers.push_back("last-modified");
    headers.push_back("link");
    headers.push_back("location");
    headers.push_back("p3p");
    headers.push_back("pragma");
    headers.push_back("preference-applied");
    headers.push_back("proxy-authenticate");
    headers.push_back("public-key-pins");
    headers.push_back("retry-after");
    headers.push_back("server");
    headers.push_back("set-cookie");
    headers.push_back("strict-transport-security");
    headers.push_back("trailer");
    headers.push_back("transfer-encoding");
    headers.push_back("tk");
    headers.push_back("upgrade");
    headers.push_back("vary");
    headers.push_back("via");
    headers.push_back("warning");
    headers.push_back("www-authenticate");
    headers.push_back("x-frame-options");
    headers.push_back("x-webkit-csp");
    headers.push_back("expect-ct");
    headers.push_back("nel");
    headers.push_back("permissions-policy");
    headers.push_back("refresh");
    headers.push_back("report-to");
    headers.push_back("status");
    headers.push_back("timing-allow-origin");
    headers.push_back("x-content-duration");
    headers.push_back("x-content-type-options");
    headers.push_back("x-powered-by");
    headers.push_back("x-redirect-by");
    headers.push_back("x-request-id");
    headers.push_back("x-ua-compatible");
    headers.push_back("x-xss-protection");
    lowcase(header);
    std::vector<std::string>::iterator it = std::find(headers.begin(), headers.end(), header);
    if (it == headers.end())
        return false;
    return true;
}

ParsedCGIOutput response::parseCGIOutput(std::string& filePath)
{
    fileStream.open(filePath.c_str());
    ParsedCGIOutput output;
    std::string line;
    // bool parsingHeaders = true;
    if (std::getline(fileStream, line))
    {
        if (line.empty() || line.find(":")== std::string::npos)
        {
            fileStream.seekg(0);
            return output;
        }
        std::istringstream lineStream(line);
        std::string key, value;
        std::getline(lineStream, key, ':');
        std::getline(lineStream, value);
        lowcase(key);
        if (!setHeadet(key))
        {
            fileStream.seekg(0);
            return output;
        }
        if (key == "status")
            output.status = std::atoi(value.c_str());
        else
            output.headers[key] = value;
    }
    int count  = 1;
    while (std::getline(fileStream, line))
    {
        if (line.empty() || (line[0] == '\r' && line.size() == 1))
            return output;
        std::istringstream lineStream(line);
        std::string key, value;
        std::getline(lineStream, key, ':');
        std::getline(lineStream, value);
        lowcase(key);
        if (key == "status")
            output.status = std::atoi(value.c_str());
        else
            output.headers[key] = value;
        if (count == 500)
        {
            fileStream.seekg(0);
            return output;
        }
        count++;
    }

    // while (std::getline(file, line))
    // {
    //     if (!line.empty() && line[line.length() - 1] == '\r')
    //         line.erase(line.size() - 1);
    //     output.body += line + "\n";
    // }
    return output;
}

int response::sendResponseFromCGI(int clientSocket, ParsedCGIOutput& cgiOutput, response& res)
{
    if (res.waitCgi)
    {
        int status;
        // std::cout << "\033[32m" <<  "i'm waiting" << "\033[0m" << std::endl;
        if (waitpid(res.pid,&status, WNOHANG))
        {
            res.pid = 0;
            if (WEXITSTATUS(status) == 150)
            {
                // serveErrorPage(clientSocket, 500, res.info);
                return 500;
            }
            res.waitCgi = false;
        }
        else
        {
            // std::cout << "end Time: " << (std::clock() - res.startTime) / CLOCKS_PER_SEC << std::endl;
            if ((std::clock() - res.startTime) / CLOCKS_PER_SEC > 10)
            {
                // serveErrorPage(clientSocket, 408, res.info);
                kill(res.pid, SIGKILL);
                waitpid(res.pid,&status, 0);
                res.pid = 0;
                return 404;
            }
        }
    }
    if (!res.waitCgi)
    {
        // std::cerr << "file " << filePath << " is open : " << res.fileStream.is_open() << std::endl;
        // std::cerr << "i'm sending cgi: " << cgiOutput.check << std::endl;
        if(!cgiOutput.check)
        {
            // std::cerr << "+++++headers" << std::endl;
            cgiOutput = parseCGIOutput(res.filePath);
            std::ostringstream responseHeaders;
            responseHeaders << "HTTP/1.1 " << cgiOutput.status << " " << getStatusMessage(cgiOutput.status) << "\r\n";
            std::map<std::string, std::string>::const_iterator iter = cgiOutput.headers.begin();
            for (; iter != cgiOutput.headers.end(); iter++)
            {
                std::cout << "fisr: " << iter->first << std::endl;
                responseHeaders << iter->first << ": " << iter->second << "\r\n";
            }
            responseHeaders << "\r\n";
            std::string headersStr = responseHeaders.str();
            std::cout << "------>5: " << headersStr << std::endl;
            send(clientSocket, headersStr.c_str(), headersStr.length(), 0);
            cgiOutput.check = 1;
            res.status = response::InProgress;
        }
        else if (res.status == response::InProgress || cgiOutput.check == 1)
        {
            // std::string chunk = getNextChunk(res, 2048);
            char buffer[2048];
            res.fileStream.read(buffer, 2048);
            int k = send(clientSocket, buffer, res.fileStream.gcount(), 0);
            // std::cerr << "+++++body => " << res.fileStream.gcount() << std::endl;
            if (k < 0 || res.fileStream.gcount() < 2048)
            {
                res.status = response::Complete;
                return 200;
            }
        }
    }
    return 200;
}

void connection::handleRequestGET(int clientSocket, Request& request,const informations& serverConfig)
{
    location routeConfig;
    response responseData;
    responseData.info = serverConfig;
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
        std::string filePath2;
        try
        {
            filePath2 = mapUriToFilePath(request.headers["uri"], routeConfig);
        
        } catch (...)
        {
            serveErrorPage(clientSocket, 403, serverConfig);
        }
        std::string filePath = filePath2;
        if (filePath == "dkhal")
        {
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
                serveErrorPage(clientSocket, 403, serverConfig);
                return;
            }
        }
        else
        {
            serveErrorPage(clientSocket, 404, serverConfig);
            return;
        }
        std::string executer = GetExtentions(filePath);
        if (routeConfig.cgi.at("cgi") == "on" && executer != "NormalFile")
        {
            try
            {
                request.cgiInfo.script = filePath;
                request.cgiInfo.cookies = request.headers["cookie"];
                request.cgiInfo.binary = executer;
                std::cout << "queris: " << request.cgiInfo.queries << std::endl;
                std::cout << "----------->1\n";
                cgiFile(request.cgiInfo);
                
                std::cout << "----------->2\n";
                filePath = request.cgiInfo.output;
                
                if (request.cgiInfo.pid == -1)
                {
                    serveErrorPage(clientSocket, 500, serverConfig);
                    return;
                }
                std::string cgiOutputFilePath = filePath;
                std::cout << "----------->3: " << filePath << std::endl;
                responseData.filePath = filePath;
                // ParsedCGIOutput cgiOutput = responseData.parseCGIOutput(cgiOutputFilePath);
                std::cout << "----------->4\n";
                // cgiOutput.filepath = filePath;
                request.storeHeader = true;
                request.cgiGET = true;
                responseData.waitCgi = true;
                responseData.pid = request.cgiInfo.pid;
                responseData.startTime = request.cgiInfo.startTime;
                Response[clientSocket] = responseData;
                // this->Cgires[clientSocket] = cgiOutput;
            }
            catch(const char *err)
            {
                std::cerr << err << std::endl;
            }
            std::cerr << "The File Geted By CGI Is: " + filePath << std::endl;
        }
        else
        {
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
                    serveErrorPage(clientSocket, 403, serverConfig);
            }
            else
            {
                responseD = "HTTP/1.1 200 OK\r\n";
                responseD += "Content-Type: " + getMimeType(filePath) + "\r\n";
                /*-------------- yachaab code ended -----------------*/
            }
            responseData.filePath = filePath;
            responseData.setResponseHeader(responseD);
            request.storeHeader = true;
            Response[clientSocket] = responseData;
        }
    }
}