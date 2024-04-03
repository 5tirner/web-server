#include "../include/mainHeader.hpp"
#include <unistd.h>

void connection::serveErrorPage(int clientSocket, int errorCode, const informations& serverConfig)
{
    std::string responseHeader;
    std::string responseBody;
    std::string foundPath;
    std::map<std::string, int>::const_iterator it = serverConfig.errorPages.begin();
    for (; it != serverConfig.errorPages.end(); ++it)
    {
        if (it->second == errorCode)
        {
            foundPath = it->first;
            break;
        }
    }
    if (!foundPath.empty()&& isRegularFile(foundPath))
    {
        std::ifstream errorPageFile(foundPath.c_str(), std::ifstream::in);
        if (errorPageFile.is_open())
        {
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
    responseHeader = "HTTP/1.1 " + to_string(errorCode) + " " + codeMsg.statMsg[errorCode] + "\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: " + to_string(responseBody.length()) + "\r\n"
                     "\r\n";

    send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
    send(clientSocket, responseBody.c_str(), responseBody.size(), 0);
}
