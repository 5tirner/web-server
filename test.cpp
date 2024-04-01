#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

struct ParsedCGIOutput
{
    std::map<std::string, std::string> headers;
    std::string body;
    int status;

    ParsedCGIOutput() : status(200) {}
};

ParsedCGIOutput parseCGIOutput(const std::string& filePath)
{
    std::ifstream file(filePath);
    ParsedCGIOutput output;
    std::string line;
    bool parsingHeaders = true;

    while (std::getline(file, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.pop_back();
        if (parsingHeaders)
        {
            if (line.empty())
                parsingHeaders = false;
            else
            {
                std::istringstream lineStream(line);
                std::string key, value;
                std::getline(lineStream, key, ':');
                std::getline(lineStream, value);
                if (!value.empty() && value.front() == ' ')
                    value.erase(0, 1);
                if (key == "Status")
                    output.status = std::atoi(value.c_str());
                else
                    output.headers[key] = value;
            }
        }
        else
            output.body += line + "\n";
    }
    while (std::getline(file, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.pop_back();
        output.body += line + "\n";
    }
    return output;
}

#include <sys/socket.h> // for send()
#include <string>
#include <map>

std::string getStatusMessage(int statusCode)
{
    std::map<int, std::string> statusMessages =
    {
        {200, "OK"},
        {201, "Created"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {413, "Payload Too Large"},
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"}
        // Add other status codes and messages as needed
    };

    // Find the status code in the map and return the corresponding message.
    // If the status code is not found, return "Status Unknown".
    if (statusMessages.find(statusCode) != statusMessages.end())
        return statusMessages[statusCode];
    else
        return "Status Unknown";
}

void sendResponseFromCGI(int clientSocket, ParsedCGIOutput& cgiOutput)
{
    std::ostringstream responseHeaders;
    // Construct status line
    responseHeaders << "HTTP/1.1 " << cgiOutput.status << " " << getStatusMessage(cgiOutput.status) << "\r\n";

    // Iterate through headers map to send each header
    for (std::map<std::string, std::string>::const_iterator iter = cgiOutput.headers.begin(); iter != cgiOutput.headers.end(); ++iter)
        responseHeaders << iter->first << ": " << iter->second << "\r\n";

    // End headers section
    responseHeaders << "\r\n";

    // Send headers
    std::string headersStr = responseHeaders.str();
    send(clientSocket, headersStr.c_str(), headersStr.length(), 0);

    // Send body
    send(clientSocket, cgiOutput.body.c_str(), cgiOutput.body.length(), 0);
}
