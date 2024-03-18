#include "../include/mainHeader.hpp"

void connection::serveErrorPage(int clientSocket, int errorCode, const informations& serverConfig) {
    std::map<int, std::vector<std::string> >::const_iterator errorPageEntry = serverConfig.error_page.find(errorCode);
    std::string responseHeader;
    std::string responseBody;

    // Check if a custom error page is found and is not empty
    if (errorPageEntry != serverConfig.error_page.end() && !errorPageEntry->second.empty())
    {
        std::string errorPagePath = errorPageEntry->second[0];
        std::ifstream errorPageFile(errorPagePath.c_str(), std::ifstream::in);
        if (errorPageFile)
        {
            // Custom error page file found, use its contents
            responseBody.assign((std::istreambuf_iterator<char>(errorPageFile)), std::istreambuf_iterator<char>());
            errorPageFile.close();
        }
        else
        {
            responseBody = "<!DOCTYPE html>"
                           "<html><head><style>"
                           "body { background-color: #f0f0f0; color: #333; font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; padding-top: 100px; text-align: center; }"
                           "h1 { font-size: 55px; font-weight: 500; }"
                           "p { font-size: 29px; }"
                           "</style></head><body>"
                           "<h1>Error " + to_string(errorCode) + " - " + codeMsg.statMsg[errorCode] + "</h1>"
                           "<p>The requested URL was not found on this server.</p>"
                           "</body></html>";
        }
    }
    else
    {
        responseBody = "<!DOCTYPE html>"
                       "<html><head><style>"
                       "body { background-color: #f0f0f0; color: #333; font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; padding-top: 100px; text-align: center; }"
                       "h1 { font-size: 55px; font-weight: 500; }"
                       "p { font-size: 29px; }"
                       "</style></head><body>"
                       "<h1>Error " + to_string(errorCode) + " - " + codeMsg.statMsg[errorCode] + "</h1>"
                       "<p>The requested URL was not found on this server.</p>"
                       "</body></html>";
    }
    // Prepare and send the response header
    responseHeader = "HTTP/1.1 " + to_string(errorCode) + " " + codeMsg.statMsg[errorCode] + "\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: " + to_string(responseBody.length()) + "\r\n"
                     "\r\n";

    send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
    send(clientSocket, responseBody.c_str(), responseBody.size(), 0);
}
