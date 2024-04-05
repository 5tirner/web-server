#include "../include/mainHeader.hpp"
#include <unistd.h>

void connection::serveErrorPage(int clientSocket, int errorCode, const informations& serverConfig)
{
    std::string responseHeader;
    std::string responseBody;
    std::string foundPath;

    if ( serverConfig.errorPages.find( errorCode )  != serverConfig.errorPages.end() )
        foundPath = serverConfig.errorPages.at( errorCode );
    if (!foundPath.empty() && isRegularFile(foundPath) && !access(foundPath.c_str(), R_OK) )
    {
        std::ifstream errorPageFile(foundPath.c_str(), std::ifstream::in);
        if (errorPageFile.is_open())
        {
            // std::cerr << "Reach The Expected THROW" << std::endl;
            responseBody.assign((std::istreambuf_iterator<char>(errorPageFile)), std::istreambuf_iterator<char>());
            errorPageFile.close();
            // std::cerr << "SKIP IT" << std::endl;
        }
        else
        {
            responseBody = "<!DOCTYPE html>"
                           "<html><head><style>"
                           "body { background-color: #f0f0f0; color: #333; font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; padding-top: 100px; text-align: center; }"
                           "h1 { font-size: 55px; font-weight: 500; }"
                           "p { font-size: 29px; }"
                           "</style></head><body>"
                           "<h1> " + to_string(errorCode == -1 ? 501 : errorCode) + " - " + codeMsg.statMsg[errorCode == -1 ? 501 : errorCode] + "</h1>"
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
                       "<h1> " + to_string(errorCode) + " - " + codeMsg.statMsg[errorCode] + "</h1>"
                       "<p>The requested URL was not found on this server.</p>"
                       "</body></html>";
    }
    responseHeader = "HTTP/1.1 " + to_string(errorCode) + " " + codeMsg.statMsg[errorCode] + "\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Length: " + to_string(responseBody.length()) + "\r\n"
                     "\r\n";
    send(clientSocket, responseHeader.c_str(), responseHeader.size(), 0);
    if ( errorCode != -1 )
        send(clientSocket, responseBody.c_str(), responseBody.size(), 0);
    this->Response[clientSocket].status = response::Complete;
}
