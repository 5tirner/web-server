#include "../include/mainHeader.hpp"
#include <fcntl.h>
#include <iterator>
#include <sys/wait.h>

response::clientResponse() : totalSize(0), bytesSent(0), status(Pending)
{
    filePath = "";
    responseHeader = "";
    waitCgi = true;
    startTime = 0;
}

response::~clientResponse()
{
    // if (pid > 0)
    // {
    //     kill(pid, SIGKILL);
    //     waitpid(pid,0, 0);
    // }
}
response::clientResponse(const clientResponse& other)
{
    filePath = other.filePath;
    totalSize = other.totalSize;
    bytesSent = other.bytesSent;
    status = other.status;
    waitCgi = other.waitCgi;
    info = other.info;
    pid = other.pid;
    startTime = other.startTime;
    responseHeader = other.responseHeader;
    removeFiles = other.removeFiles;
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
        waitCgi = other.waitCgi;
        info = other.info;
        pid = other.pid;
        startTime = other.startTime;
        removeFiles = other.removeFiles;
    }
    return *this;
}
void response::setResponseHeader(const std::string& header)
{
    responseHeader = header;
}

void openFile(response& res , const std::string& path)
{
    res.filePath = path;
    res.fileStream.open(path.c_str(), std::ifstream::binary);
    if (!res.fileStream.is_open())
    {
        std::cerr << "File Not Opened" << std::endl;
        // if (res.totalSize == std::string::npos)
        //     return ;
        // sendErrorResponse(clientSocket, 404, "Not Found");
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
    std::string buf(buffer, res.fileStream.gcount());
    if (bytesRead <  chunkSize || res.fileStream.eof())
    {
        res.status = res.Complete;
        res.fileStream.close();
    }
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
        //std::cout << "RESPONSE HEADERS" << std::endl;
        if (respData.fileStream)
            respData.fileStream.close();
        if (!respData.filePath.empty())
            openFile(respData ,respData.filePath);
        if (!respData.responseHeader.empty() && !isDirectory(respData.filePath))
            respData.responseHeader += "Content-Length: " + to_string(respData.totalSize) + "\r\n\r\n";
        int k = send(clientSocket, respData.responseHeader.c_str(), respData.responseHeader.size(), 0);
        if (k < 0)
        {
            respData.status = response::Complete;
            return ;
        }
        if (respData.filePath.empty() || respData.responseHeader.find("Location:") != std::string::npos)
            respData.status = response::Complete;
        else
            respData.status = response::InProgress;
    }
    else if (respData.status == response::InProgress)
    {
        //std::cout << "RESPONSE BODY" << std::endl;

        std::string chunk = getNextChunk(respData, 2048); 
        if (!chunk.empty())
        {
            int k = send(clientSocket, chunk.c_str(), chunk.size(), 0);
            if (k < 0)
            {
                respData.status = response::Complete;
                return ;
            }
        }
        // Check if file reading is complete
        if (!hasNextChunk(respData))
        {
            closeFile(respData);
            respData.status = response::Complete;
        }
    }
}

