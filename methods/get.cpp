#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <linux/limits.h>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>



#include <string>
#include <map>

std::string getMimeType(std::string& filePath)
{
    std::map<std::string, std::string> mimeTypes;

    // Directly assign each MIME type
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".shtml"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".xml"] = "text/xml";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".atom"] = "application/atom+xml";
    mimeTypes[".rss"] = "application/rss+xml";
    mimeTypes[".mml"] = "text/mathml";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".jad"] = "text/vnd.sun.j2me.app-descriptor";
    mimeTypes[".wml"] = "text/vnd.wap.wml";
    mimeTypes[".htc"] = "text/x-component";
    mimeTypes[".avif"] = "image/avif";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".svgz"] = "image/svg+xml";
    mimeTypes[".tif"] = "image/tiff";
    mimeTypes[".tiff"] = "image/tiff";
    mimeTypes[".wbmp"] = "image/vnd.wap.wbmp";
    mimeTypes[".webp"] = "image/webp";
    mimeTypes[".ico"] = "image/x-icon";
    mimeTypes[".jng"] = "image/x-jng";
    mimeTypes[".bmp"] = "image/x-ms-bmp";
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".jar"] = "application/java-archive";
    mimeTypes[".war"] = "application/java-archive";
    mimeTypes[".ear"] = "application/java-archive";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".hqx"] = "application/mac-binhex40";
    mimeTypes[".doc"] = "application/msword";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".ps"] = "application/postscript";
    mimeTypes[".eps"] = "application/postscript";
    mimeTypes[".ai"] = "application/postscript";
    mimeTypes[".rtf"] = "application/rtf";
    mimeTypes[".m3u8"] = "application/vnd.apple.mpegurl";
    mimeTypes[".kml"] = "application/vnd.google-earth.kml+xml";
    mimeTypes[".kmz"] = "application/vnd.google-earth.kmz";
    mimeTypes[".xls"] = "application/vnd.ms-excel";
    mimeTypes[".eot"] = "application/vnd.ms-fontobject";
    mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
    mimeTypes[".odg"] = "application/vnd.oasis.opendocument.graphics";
    mimeTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
    mimeTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    mimeTypes[".odt"] = "application/vnd.oasis.opendocument.text";
    mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeTypes[".wmlc"] = "application/vnd.wap.wmlc";
    mimeTypes[".wasm"] = "application/wasm";
    mimeTypes[".7z"] = "application/x-7z-compressed";
    mimeTypes[".cco"] = "application/x-cocoa";
    mimeTypes[".jardiff"] = "application/x-java-archive-diff";
    mimeTypes[".jnlp"] = "application/x-java-jnlp-file";
    mimeTypes[".run"] = "application/x-makeself";
    mimeTypes[".pl"] = "application/x-perl";
    mimeTypes[".pm"] = "application/x-perl";
    mimeTypes[".prc"] = "application/x-pilot";
    mimeTypes[".pdb"] = "application/x-pilot";
    mimeTypes[".rar"] = "application/x-rar-compressed";
    mimeTypes[".rpm"] = "application/x-redhat-package-manager";
    mimeTypes[".sea"] = "application/x-sea";
    mimeTypes[".swf"] = "application/x-shockwave-flash";
    mimeTypes[".sit"] = "application/x-stuffit";
    mimeTypes[".tcl"] = "application/x-tcl";
    mimeTypes[".tk"] = "application/x-tcl";
    mimeTypes[".der"] = "application/x-x509-ca-cert";
    mimeTypes[".pem"] = "application/x-x509-ca-cert";
    mimeTypes[".crt"] = "application/x-x509-ca-cert";
    mimeTypes[".xpi"] = "application/x-xpinstall";
    mimeTypes[".xhtml"] = "application/xhtml+xml";
    mimeTypes[".xspf"] = "application/xspf+xml";
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".bin"] = "application/octet-stream";
    mimeTypes[".exe"] = "application/octet-stream";
    mimeTypes[".dll"] = "application/octet-stream";
    mimeTypes[".deb"] = "application/octet-stream";
    mimeTypes[".dmg"] = "application/octet-stream";
    mimeTypes[".iso"] = "application/octet-stream";
    mimeTypes[".img"] = "application/octet-stream";
    mimeTypes[".msi"] = "application/octet-stream";
    mimeTypes[".msp"] = "application/octet-stream";
    mimeTypes[".msm"] = "application/octet-stream";
    mimeTypes[".mid"] = "audio/midi";
    mimeTypes[".midi"] = "audio/midi";
    mimeTypes[".kar"] = "audio/midi";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".ogg"] = "audio/ogg";
    mimeTypes[".m4a"] = "audio/x-m4a";
    mimeTypes[".ra"] = "audio/x-realaudio";
    mimeTypes[".3gpp"] = "video/3gpp";
    mimeTypes[".3gp"] = "video/3gpp";
    mimeTypes[".ts"] = "video/mp2t";
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".mpeg"] = "video/mpeg";
    mimeTypes[".mpg"] = "video/mpeg";
    mimeTypes[".mov"] = "video/quicktime";
    mimeTypes[".webm"] = "video/webm";
    mimeTypes[".flv"] = "video/x-flv";
    mimeTypes[".m4v"] = "video/x-m4v";
    mimeTypes[".mng"] = "video/x-mng";
    mimeTypes[".asx"] = "video/x-ms-asf";
    mimeTypes[".asf"] = "video/x-ms-asf";
    mimeTypes[".wmv"] = "video/x-ms-wmv";
    mimeTypes[".avi"] = "video/x-msvideo";

    size_t dotPos = filePath.rfind('.');
    if (dotPos != std::string::npos)
    {
        std::string extension = filePath.substr(dotPos);
        std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end())
            return it->second;
    }
    return "application/octet-stream"; // Default MIME type if not found
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
        if (!path.empty() && path[path.length() - 1] == '/' && 
            !resolvedPath.empty() && resolvedPath[resolvedPath.length() - 1] != '/')
            resolvedPath += '/';
    }
    return resolvedPath;
}

bool isPathWithinRoot(std::string& fullPath, std::string& rootPath)
{
    if (fullPath.find(rootPath) != 0)
        return false;
    return true;
}

std::string mapUriToFilePath( std::string& uri,  location locConfig, Request& request)
{
    std::string rootPath = locConfig.root.at("root");
    std::string locPath = locConfig.directory.at("location");
    std::string pathSuffix = uri.substr(locPath.length());
    std::string fullPath = rootPath;
    std::string resolvedPath;
    if (pathSuffix.empty() || pathSuffix[0] != '/')
        fullPath += "/";
    fullPath += pathSuffix;
    request.cgiInfo.script = pathSuffix;
    if (pathSuffix.find("../") != std::string::npos)
    {
        fullPath = resolveFilePath(fullPath);
        if (fullPath.empty())
            throw std::runtime_error("there is a problem");
        if (!isPathWithinRoot(fullPath, rootPath))
            throw std::runtime_error("there is a problem");
    }
    std::string indexPath;
    if ((pathSuffix.empty() || pathSuffix[pathSuffix.length() - 1] == '/'))
    {
        std::istringstream iss(locConfig.index.at("index"));
        std::string indexFile;
        while (std::getline(iss, indexFile, ' '))
        {
            indexPath = fullPath + indexFile;
            if (!indexPath.empty() && fileExists(indexPath) && isPathWithinRoot(indexPath, rootPath))
                return indexPath;
        }
    }
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
    html << "<html><head><title>Index of " << path << "</title></head><body>";
    html << "<h1>Index of " << path << "</h1><hr><pre>";

    dir = opendir(path.c_str());
    if (dir != NULL)
    {
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
void cleanupResponseFiles(std::vector<std::string>& files)
{
    for (size_t i = 0; i < files.size(); ++i)
    {
        if (remove(files[i].c_str()) != 0)
            std::cerr << "Error removing file: " << files[i] << std::endl;
    }
    files.clear();
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
            output.headers.insert((std::make_pair(key, value)));
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
            output.headers.insert((std::make_pair(key, value)));
        if (count == 500)
        {
            fileStream.seekg(0);
            return output;
        }
        count++;
    }
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
                return 408;
            }
        }
    }
    if (!res.waitCgi)
    {
        std::cerr << "file " << filePath << " is open : " << res.fileStream.is_open() << std::endl;
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
                responseHeaders << iter->first << ": " << iter->second << "\n";
            }
            responseHeaders << "\r\n";
            std::string headersStr = responseHeaders.str();
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
        routeConfig = findRouteConfig(request.headers.at("uri"), serverConfig);
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
            filePath2 = mapUriToFilePath(request.headers.at("uri"), routeConfig, request);
        
        } catch (...)
        {
            serveErrorPage(clientSocket, 403, serverConfig);
            return;
        }
        std::string filePath = filePath2;
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
        std::string executer = GetExtension(filePath);
        if (routeConfig.cgi.at("cgi") == "on" && executer != "NormalFile")
        {
            try
            {
                request.cgiInfo.script = filePath;
                size_t pos = request.cgiInfo.script.rfind("/");
                request.cgiInfo.script = request.cgiInfo.script.substr(pos + 1);
                std::cout << "scripte name: "<< request.cgiInfo.script << std::endl;
                request.cgiInfo.pathInfo = filePath;
                std::cout << "pathinfo: "<< request.cgiInfo.pathInfo << std::endl;
                request.cgiInfo.cookies = request.headers["cookie"];
                request.cgiInfo.binary = executer;
                cgiFile(request.cgiInfo);
                filePath = request.cgiInfo.output;

                responseData.removeFiles.push_back(request.cgiInfo.output);
                responseData.removeFiles.push_back( request.filename );
                if (request.cgiInfo.pid == -1)
                {
                    serveErrorPage(clientSocket, 500, serverConfig);
                    return;
                }
                std::string cgiOutputFilePath = filePath;
                responseData.filePath = filePath;
                request.storeHeader = true;
                request.cgiGET = true;
                responseData.waitCgi = true;
                responseData.pid = request.cgiInfo.pid;
                responseData.startTime = request.cgiInfo.startTime;
                Response[clientSocket] = responseData;
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
                std::string check = request.headers.at("uri") + it->index.at("index");

                std::map<std::string, std::string>::iterator autoindexIt = routeConfig.autoindex.find("autoindex");
                if (filePath[filePath.length() - 1] != '/')
                {
                    responseD = "HTTP/1.1 301 Moved Permanently\r\n";
                    responseD += "Location: " + request.headers.at("uri") + "/\r\n";
                }
                else if (isRegularFile(check))
                {
                    responseD = "HTTP/1.1 301 OK\r\n";
                    responseD += "Location: " + check + " \r\n";
                }
                else if (autoindexIt != routeConfig.autoindex.end() && autoindexIt->second == "on")
                {
                    std::string directoryContent = generateDirectoryListing(filePath);
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