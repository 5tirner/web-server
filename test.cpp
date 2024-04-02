#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <sys/socket.h> // For send()

// Assuming the existence of a 'location' struct, defined elsewhere.
struct location {};

// Assuming existence of a global or accessible map for status messages
std::map<int, std::string> codeMsg_statMsg = {
    {404, "Not Found"},
    {500, "Internal Server Error"}
    // Add additional status codes as necessary.
};

// Helper function for int to string conversion, necessary for building the HTTP response.
std::string to_string(int value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

// Definition of the 'informations' struct as per user's clarification.
struct informations {
    std::vector<location> locationsInfo;
    std::map<std::string, int> errorPages; // Map of error page path (string) to HTTP status code (int)
    std::map<std::string, std::string> limitClientBody, port, host, serverName, defaultRoot;
    std::vector<std::string> others, locations;
};

class connection {
public:
    void serveErrorPage(int clientSocket, int errorCode, const informations& serverConfig);
};

void connection::serveErrorPage(int clientSocket, int errorCode, const informations& serverConfig) {
    std::string responseHeader;
    std::string responseBody;
    std::string foundPath;

    // Iterate over the map without using auto
    for (std::map<std::string, int>::const_iterator it = serverConfig.errorPages.begin(); it != serverConfig.errorPages.end(); ++it) {
        if (it->second == errorCode) {
            foundPath = it->first;
            break;
        }
    }

    if (!foundPath.empty()) {
        // Attempt to open the file at the found path
        std::ifstream errorPageFile(foundPath.c_str(), std::ifstream::in);
        if (errorPageFile) {
            // File opened successfully, read its contents into responseBody
            responseBody.assign((std::istreambuf_iterator<char>(errorPageFile)),
                                std::istreambuf_iterator<char>());
        } else {
            // File couldn't be opened. Use a default error message.
            responseBody = "<html><body><h2>Error " + to_string(errorCode) + "</h2><p>Custom error page not found.</p></body></html>";
        }
    } else {
        // No entry found for this error code. Use a generic error message.
        responseBody = "<html><body><h2>Error " + to_string(errorCode) + "</h2><p>The requested URL was not found on this server.</p></body></html>";
    }

    // Build the HTTP response header
    responseHeader = "HTTP/1.1 " + to_string(errorCode) + " " + codeMsg_statMsg[errorCode] + "\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: " + to_string(responseBody.length()) + "\r\n\r\n";

    // Send the response header followed by the response body to the client
    send(clientSocket, responseHeader.c_str(), responseHeader.length(), 0);
    send(clientSocket, responseBody.c_str(), responseBody.length(), 0);
}
