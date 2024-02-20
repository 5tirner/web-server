#include "../include/Server.hpp"

struct FormDataPart
{
    std::map<std::string, std::string> headers;
    std::string body;
};

struct MultipartFormData
{
    std::vector<FormDataPart> parts;
};

std::map<std::string, std::string> parseUrlEncoded(const std::string& body)
{
    // std::cout << "body2: " << body << std::endl;
    std::map<std::string, std::string> data;
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&'))
    {
        // std::cout << "pair: " << pair << std::endl;
        size_t delimiter_pos = pair.find('=');
        if (delimiter_pos != std::string::npos)
        {
            std::string key = pair.substr(0, delimiter_pos);
            std::string value = pair.substr(delimiter_pos + 1);
            // std::cout << "key: " << key << " value: " << value << std::endl;
            data[key] = value;
        }
    }
    return data;
}


MultipartFormData parseMultipartFormData(const std::string& body, const std::string& boundary)
{
    MultipartFormData formData;
    std::string delimiter = "--" + boundary + "\r\n";
    std::string endDelimiter = "--" + boundary + "--";

    size_t pos = 0;
    size_t nextPos;

    while ((nextPos = body.find(delimiter, pos)) != std::string::npos) {
        size_t partEnd = body.find(delimiter, nextPos + delimiter.size());
        if (partEnd == std::string::npos) {
            partEnd = body.find(endDelimiter, nextPos + delimiter.size());
            if (partEnd == std::string::npos) break;  // End of data
        }

        std::string part = body.substr(nextPos + delimiter.size(), partEnd - nextPos - delimiter.size());

        // Parse headers and body of each part
        size_t headerEnd = part.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            FormDataPart formDataPart;

            std::string headersPart = part.substr(0, headerEnd);
            std::istringstream headerStream(headersPart);
            std::string headerLine;

            while (std::getline(headerStream, headerLine) && !headerLine.empty()) {
                size_t colonPos = headerLine.find(":");
                if (colonPos != std::string::npos) {
                    std::string key = headerLine.substr(0, colonPos);
                    std::string value = headerLine.substr(colonPos + 2, headerLine.length() - colonPos - 3); // -3 for \r\n
                    formDataPart.headers[key] = value;
                }
            }

            formDataPart.body = part.substr(headerEnd + 4); // +4 to skip \r\n\r\n
            formData.parts.push_back(formDataPart);
        }

        pos = partEnd + delimiter.size();
    }

    return formData;
}


typedef std::map<std::string, std::string> JsonData;

JsonData parseJson(const std::string& json)
{
    JsonData data;
    size_t pos = 0;

    // Remove whitespace
    std::string str;
    for (size_t i = 0; i < json.length(); ++i) {
        if (json[i] != ' ' && json[i] != '\n' && json[i] != '\t' && json[i] != '\r') {
            str += json[i];
        }
    }

    // Check if it starts and ends with curly braces
    if (str[0] != '{' || str[str.length() - 1] != '}') {
        throw std::runtime_error("Invalid JSON format");
    }

    // Remove the starting and ending curly braces
    str = str.substr(1, str.length() - 2);

    while (pos < str.length()) {
        // Find key
        size_t start = str.find('"', pos) + 1;
        size_t end = str.find('"', start);
        std::string key = str.substr(start, end - start);

        // Find value
        start = str.find('"', end + 1) + 1;
        end = str.find('"', start);
        std::string value = str.substr(start, end - start);

        // Add to map
        data[key] = value;

        pos = end + 1;
    }

    return data;
}




void Server::handleRequestPOST(int clientSocket, HTTPRequest& request)
{
    // Check Content-Type header
    std::string contentType = request.headers["content-type"].front();
    // std::cout << "POST contentType: " << contentType << std::endl;

    if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        // std::cout << "Content dkhal l application/x-www-form-urlencoded\n";
        // std::cout << "body: " << request.body << std::endl;
        std::map<std::string, std::string> formData = parseUrlEncoded(request.body);
    }
    else if (contentType.find("multipart/form-data") != std::string::npos)
    {
        // Extract boundary
        size_t boundaryPos = contentType.find("boundary=");
        if (boundaryPos == std::string::npos)
        {
            sendErrorResponse(clientSocket, 400, "Bad Request: No boundary in multipart/form-data");
            return;
        }
        std::string boundary = contentType.substr(boundaryPos + 9); // 9 is the length of "boundary="

        // Handle file uploads and multipart data
        MultipartFormData multipartData = parseMultipartFormData(request.body, boundary);
        // Handle multipart data
    }
    else if (contentType.find("application/json") != std::string::npos)
    {
        // Parse JSON data
        JsonData jsonData = parseJson(request.body);
        // Handle JSON data
    }
    else
    {
        // Unsupported Content-Type
        sendErrorResponse(clientSocket, 415, "Unsupported Media Type");
        return;
    }

    // Process the data as needed

    // Send a response to the client
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}