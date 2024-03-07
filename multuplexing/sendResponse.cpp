#include "../include/mainHeader.hpp"

response::clientResponse() : totalSize(0), bytesSent(0), status(Pending) {}

response::clientResponse(const clientResponse& other)
{
    filePath = other.filePath;
    totalSize = other.totalSize;
    bytesSent = other.bytesSent;
    status = other.status;
    responseHeader = other.responseHeader;
    // if (other.fileStream.is_open())
    //     openFile(filePath);
}
clientResponse& response::operator=(const clientResponse& other)
{
    if (this != &other)
    {
        filePath = other.filePath;
        totalSize = other.totalSize;
        bytesSent = other.bytesSent;
        status = other.status;
        responseHeader = other.responseHeader;
        // if (other.fileStream.is_open())
        //     openFile(filePath);  // Reopen the file stream
    }
    return *this;
}
void response::setResponseHeader(const std::string& header)
{
    responseHeader = header;
}

void openFile(response& res , const std::string& path)
{
    // closeFile(); // Ensure any previously opened file is closed
    res.filePath = path;
    res.fileStream.open(path.c_str(), std::ifstream::binary);
    if (!res.fileStream)
    {
        std::cout << "wa9ila dkhal l 404\n";
        // sendErrorResponse(clientSocket, 404, "Not Found");
        // return;
        res.status = res.Complete; // Mark as complete if file open failed
        return;
    }

    res.fileStream.seekg(0, std::ios::end);
    res.totalSize = res.fileStream.tellg();
    res.fileStream.seekg(0, std::ios::beg);
    res.status = res.InProgress;
}

std::string getNextChunk(response& res, size_t chunkSize)
{
    if (!res.fileStream.is_open() || res.status != res.InProgress)
        return "";
    char buffer[chunkSize];
    res.fileStream.read(buffer, chunkSize);
    size_t bytesRead = res.fileStream.gcount();
    res.bytesSent += bytesRead;
    if (bytesRead <  chunkSize || res.fileStream.eof())
    {
        res.status = res.Complete;
        res.fileStream.close();
    }
    std::string buf(buffer, 2048);
    return buf;
}

bool hasNextChunk(response& res)
{
    if (res.fileStream.is_open())
    {
        if (res.bytesSent < res.totalSize)
            return true;
        else
            return false;
    }
    else
        return false;
}

void closeFile(response& res)
{
    if (res.fileStream.is_open())
        res.fileStream.close();
    res.status = res.Complete;
}

void sendResponseChunk(int clientSocket, response& respData)
{
    if (respData.status == response::Complete)
        return;

    // Send the response header if needed
    if (respData.status == response::Pending)
    {
        openFile(respData ,respData.filePath); // Ensure to provide file path
        
        respData.responseHeader += "Content-Length: " + to_string(respData.totalSize) + "\r\n\r\n";
        send(clientSocket, respData.responseHeader.c_str(), respData.responseHeader.size(), 0);
        respData.status = response::InProgress;
    }
    else if (respData.status == response::InProgress)
    {
        // Assuming getNextChunk is a method that reads the next part of the file
        //     send(clientSocket, chunk.c_str(), chunk.size(), 0);
        std::string chunk = getNextChunk(respData, 2048); 
        // std::cout << chunk << std::endl;
        if (!chunk.empty())
            send(clientSocket, chunk.c_str(), chunk.size(), 0);

        // Check if file reading is complete
        if (!hasNextChunk(respData))
        {
            std::string lastChunk = "0\r\n\r\n";
            send(clientSocket, lastChunk.c_str(), lastChunk.size(), 0);
            closeFile(respData);
            respData.status = response::Complete;

        }
    }
}