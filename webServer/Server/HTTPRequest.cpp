#include "../include/HTTPRequest.hpp"

bool readLine(std::istringstream& requestStream, std::string& line)
{
    std::getline(requestStream, line);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    return requestStream.good();
}

HTTPRequest::HTTPRequest() : method(""), uri(""), httpVersion(""), body(""), rawRequest("") {}


void toLowerCase(std::string& s)
{
    for (size_t i = 0; i < s.length(); ++i)
        s[i] = std::tolower(s[i]);
}


void HTTPRequest::parse(std::string& rawRequest)
{
    std::istringstream requestStream(rawRequest);
    std::string line;
    // std::cout << "from Parse pro\n";
    if (!readLine(requestStream, line))
        throw std::runtime_error("Wrong request line");
    std::istringstream requestLineStream(line);
    if (!(requestLineStream >> method >> uri >> httpVersion))
        throw std::runtime_error("Wrong request line");
    // std::cout << "===uri: " << uri << std::endl;
    size_t queryPos = uri.find('?');
    if (queryPos != std::string::npos)
    {
        std::string queryString = uri.substr(queryPos + 1);
        uri = uri.substr(0, queryPos);
        std::istringstream queryStream(queryString);
        std::string param;
        while (std::getline(queryStream, param, '&')) {
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
            toLowerCase(key);
            headers[key].push_back(value.substr(value.find_first_not_of(" ")));
        }
        else
            throw std::runtime_error("Wrong header line");
    }
    // std::cout << "try to be pro and read the body\n";

    // Determine body parsing strategy
    bool contentLengthHeaderFound = headers.find("content-length") != headers.end();
    bool transferEncodingHeaderFound = headers.find("transfer-encoding") != headers.end() && !headers["transfer-encoding"].empty();

    // Parse body based on headers
    if (contentLengthHeaderFound)
    {
        int length = std::atoi(headers["content-length"][0].c_str());
        if (length > 0)
        {
            std::vector<char> buffer(length);
            requestStream.read(&buffer[0], length);
            body.assign(buffer.begin(), buffer.end());
        }
    }
    else if (transferEncodingHeaderFound && headers["transfer-encoding"][0] == "chunked")
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
void HTTPRequest::appendData(const char* buffer, int length)
{
    this->rawRequest.append(buffer, length);
}

bool HTTPRequest::isComplete()
{
    std::cout << "this->method  " << this->method << std::endl;
    // Check if the end of the headers section is found
    size_t headerEnd = this->rawRequest.find("\r\n\r\n");
    size_t headerEnd2 = this->rawRequest.find("\n\n");
    std::cout << headerEnd2 << std::endl;
    // size_t headerEnd2 = this->rawRequest.find("\n\n");
    if (headerEnd == std::string::npos && headerEnd2 == std::string::npos)
        return false; // The end of headers not yet received
    // Check for "Content-Length" header if the method is POST or PUT
    std::cout << "this->methodssds  " << this->method << std::endl;
    if (this->method == "POST")
    {
        std::cout << "rah dkhal l post\n";
        size_t contentLengthHeader = this->rawRequest.find("Content-Length: ");
        if (contentLengthHeader != std::string::npos)
        {
            size_t start = contentLengthHeader + 16; // Length of "Content-Length: "
            size_t end = this->rawRequest.find("\r\n", start);
            int contentLength = std::atoi(this->rawRequest.substr(start, end - start).c_str());

            // Check if the body length is equal to the Content-Length value
            size_t bodyStart = headerEnd + 4; // Length of "\r\n\r\n"
            // size_t bodyStart2 = headerEnd2 + 2; // Length of "\r\n\r\n"
            if (this->rawRequest.length() - bodyStart < static_cast<size_t>(contentLength))
                return false; // The body is not fully received
        }
    }

    return true; // Headers and body (if applicable) are fully received
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
