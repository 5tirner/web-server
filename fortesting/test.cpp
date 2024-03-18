#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <sstream>

// Checks if a file exists
bool fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

// Checks if a path is a directory
bool isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) return false;
    return S_ISDIR(statbuf.st_mode);
}

// Generates a simple HTML directory listing
std::string generateDirectoryListing(const std::string& path) {
    std::ostringstream listing;
    listing << "<html><head><title>Index of " << path << "</title></head><body>";
    listing << "<h1>Index of " << path << "</h1><ul>";

    DIR* dir = opendir(path.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            listing << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>";
        }
        closedir(dir);
    }
    listing << "</ul></body></html>";
    return listing.str();
}

void connection::handleRequestGET(int clientSocket, Request& request, const informations& serverConfig) {
    location routeConfig = findRouteConfig(request.headers["uri"], serverConfig);
    
    // Ensure the method is allowed for this route
    if (routeConfig.allowed_methodes.find("GET") == std::string::npos) {
        // Method not allowed response
        return;
    }

    std::string filePath = mapUriToFilePath(request.headers["uri"], routeConfig);

    // Serve the file or directory listing
    if (fileExists(filePath)) {
        if (isDirectory(filePath)) {
            if (routeConfig.autoindex == "on") {
                std::string content = generateDirectoryListing(filePath);
                // Send directory listing
            } else {
                filePath += "/index.html"; // Default file to serve if autoindex is off
                if (!fileExists(filePath)) {
                    // Not found response
                    return;
                }
                // Serve the file
            }
        } else {
            // Serve the file
        }
    } else {
        // Not found response
        return;
    }

    // File serving logic here (omitted for brevity)
}

std::string mapUriToFilePath(const std::string& uri, const location& routeConfig) {
    std::string filePath = routeConfig.root;
    std::string requestPath = uri.substr(routeConfig.directory.size());

    // Normalize the root path
    if (filePath.back() != '/') filePath += '/';
    if (requestPath.front() == '/') requestPath.erase(0, 1);

    filePath += requestPath;

    // Check for directory request to append index file
    if (filePath.back() == '/' || isDirectory(filePath)) {
        std::string indexFile = routeConfig.index.empty() ? "index.html" : routeConfig.index;
        filePath += indexFile;
    }

    return filePath;
}
