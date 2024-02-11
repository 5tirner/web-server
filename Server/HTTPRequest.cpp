#include "../include/Server.hpp"

bool readLine(std::istringstream& requestStream, std::string& line)
{
    std::getline(requestStream, line);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    return requestStream.good();
}

HTTPRequest::HTTPRequest() : method(""), uri(""), httpVersion(""), body(""), rawRequest("") {}

void HTTPRequest::appendData(const char* buffer, int length)
{
    this->rawRequest.append(buffer, length);
}

bool HTTPRequest::isComplete()
{
    // Check if the end of the headers section is found
    size_t headerEnd = this->rawRequest.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false; // The end of headers not yet received

    // Check for "Content-Length" header if the method is POST or PUT
    if (this->method == "POST")
    {
        size_t contentLengthHeader = this->rawRequest.find("Content-Length: ");
        if (contentLengthHeader != std::string::npos)
        {
            size_t start = contentLengthHeader + 16; // Length of "Content-Length: "
            size_t end = this->rawRequest.find("\r\n", start);
            int contentLength = std::stoi(this->rawRequest.substr(start, end - start));

            // Check if the body length is equal to the Content-Length value
            size_t bodyStart = headerEnd + 4; // Length of "\r\n\r\n"
            if (this->rawRequest.length() - bodyStart < static_cast<size_t>(contentLength))
                return false; // The body is not fully received
        }
    }

    return true; // Headers and body (if applicable) are fully received
}

void HTTPRequest::parse(std::string& rawRequest)
{
    std::istringstream requestStream(rawRequest);
    std::string line;
    std::cout << "from Prse pro\n";
    if (!readLine(requestStream, line))
        throw std::runtime_error("Wrong request line");
    std::istringstream requestLineStream(line);
    if (!(requestLineStream >> method >> uri >> httpVersion))
        throw std::runtime_error("Wrong request line");

    size_t queryPos = uri.find('?');
    if (queryPos != std::string::npos)
    {
        std::string queryString = uri.substr(queryPos + 1);
        uri = uri.substr(0, queryPos);
        std::istringstream queryStream(queryString);
        std::string param;
        while (std::getline(queryStream, param, '&'))
        {
            size_t equalPos = param.find('=');
            if (equalPos != std::string::npos)
                queryParams[param.substr(0, equalPos)] = param.substr(equalPos + 1);
        }
    }

    // Parse headers
    while (readLine(requestStream, line) && !line.empty())
    {
        std::istringstream headerLineStream(line);
        std::string key, value;
        if (std::getline(headerLineStream, key, ':') && std::getline(headerLineStream, value))
        {
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            headers[key].push_back(value.substr(value.find_first_not_of(" "))); // Trim leading whitespace
        }
        else
            throw std::runtime_error("Wrong header line");
    }
    std::cout << "try to be pro and read the body\n";

        // Determine body parsing strategy
    bool contentLengthHeaderFound = headers.find("content-length") != headers.end();
    // bool transferEncodingHeaderFound = headers.find("transfer-encoding") != headers.end() && !headers["transfer-encoding"].empty();

    // Parse body based on headers
    if (contentLengthHeaderFound)
    {
        int length = std::atoi(headers["content-length"][0].c_str());
        if (length > 0) {
            std::vector<char> buffer(length);
            requestStream.read(&buffer[0], length);
            body.assign(buffer.begin(), buffer.end());
        }
    }
    else if (headers.find("transfer-encoding") != headers.end() && headers["transfer-encoding"][0] == "chunked")
    {
        while (true)
        {
            std::string chunkSizeStr;
            std::getline(requestStream, chunkSizeStr);
            unsigned int chunkSize;
            std::istringstream(chunkSizeStr) >> std::hex >> chunkSize;
            if (chunkSize == 0)
                break;
            std::vector<char> buffer(chunkSize);
            requestStream.read(&buffer[0], chunkSize);
            body.append(buffer.begin(), buffer.end());
            // Consume trailing newline
            std::string temp;
            std::getline(requestStream, temp);
        }
    }
}
void HTTPRequest::clear()
{
    // Clear all the member variables to their default state
    this->method.clear();
    this->uri.clear();
    this->httpVersion.clear();
    this->headers.clear();
    this->body.clear();
    this->queryParams.clear();
    this->rawRequest.clear();
    // Any other member variables you have
}

std::string HTTPRequest::getFullRequest() const
{
        std::string fullRequest;
        fullRequest += "Method: " + method + "\n";
        fullRequest += "URI: " + uri + "\n";
        fullRequest += "HTTP Version: " + httpVersion + "\n";
        fullRequest += "Headers:\n";
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            for (std::vector<std::string>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
                fullRequest += it->first + ": " + *vit + "\n";
            }
        }
        
        fullRequest += "Body:\n" + body;
        return fullRequest;
}

std::map<std::string, std::string> parseUrlEncoded(const std::string& body)
{
    std::cout << "body2: " << body << std::endl;
    std::map<std::string, std::string> data;
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&'))
    {
        std::cout << "pair: " << pair << std::endl;
        size_t delimiter_pos = pair.find('=');
        if (delimiter_pos != std::string::npos)
        {
            std::string key = pair.substr(0, delimiter_pos);
            std::string value = pair.substr(delimiter_pos + 1);
            std::cout << "key: " << key << " value: " << value << std::endl;
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
    std::cout << "POST contentType: " << contentType << std::endl;

    if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        std::cout << "Content dkhal l application/x-www-form-urlencoded\n";
        std::cout << "body: " << request.body << std::endl;
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

