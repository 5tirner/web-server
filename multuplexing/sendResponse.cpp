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
        if (respData.fileStream)
            respData.fileStream.close();
        if (!respData.filePath.empty())
            openFile(respData ,respData.filePath);
        if (!respData.responseHeader.empty())
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
            // std::string lastChunk = "0\r\n\r\n";
            // send(clientSocket, lastChunk.c_str(), lastChunk.size(), 0);
            closeFile(respData);
            respData.status = response::Complete;
        }
    }
}

static std::string readHtmlFile( const char* filepath )
{
    std::ifstream file( filepath );
    if ( !file.is_open() )
        std::exception();
    std::string content( ( std::istreambuf_iterator<char>( file )  ), ( std::istreambuf_iterator<char>() ) );
    file.close();
    return ( content );
}

static std::string replacePlaceholder( const std::string& html, const std::string& msgPlaceholder ,
                                const std::string& codePlaceholder, const std::string& msgReplace,
                                const std::string& codeReplace )
{
    std::string result  = html;
    size_t msgPlhPos    = result.find( msgPlaceholder );
    size_t codePlhPos   = result.find( codePlaceholder );

    if ( msgPlhPos != std::string::npos && codePlhPos != std::string::npos )
    {
        result.replace( codePlhPos, codePlaceholder.length(), codeReplace );
        result.replace( msgPlhPos, msgPlaceholder.length(), msgReplace );
    }
    return ( result );
}

std::string creatTemplate( const char* filepath, int& statcode, code& msgCode )
{
    try
    {
        std::string version( "HTTP/1.1" );
        std::string htmlTemplate( readHtmlFile( filepath ) );
        std::string modifiedHtml( replacePlaceholder( htmlTemplate, "{msg}", "{code}",
                                  msgCode.statMsg.at(statcode) , to_string( statcode ) ) ); //! check to_string stander
        std::string httpResponse    = version + " " + to_string( statcode ) + " " + msgCode.statMsg.at(statcode) + "\r\n"; //! check to_string stander
        httpResponse                += "Content-Type: text/html\r\n";
        httpResponse                += "Content-Length: " + to_string(modifiedHtml.length()) + "\r\n\r\n";
        httpResponse                += modifiedHtml;
        return ( httpResponse );
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ("");
}

void    sendResponse( int& fd, const std::string& response )
{
    int rc = write( fd, response.c_str(), response.length() );
    if ( rc < 0 )
        throw std::exception();
}
