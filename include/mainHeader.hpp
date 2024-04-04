#ifndef MAINHEADER_HPP
#define MAINHEADER_HPP
#include <map>
#include <ctime>
#include <string>
#include <vector>
#include <cstdio>
#include <cstddef>
#include <climits>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sstream>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <algorithm>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class   configFile
{
    private:
        std::string  fileName;
        std::fstream File;
    public:
        configFile(void);
        configFile(int ac, char **av);
        configFile(const configFile &other);
        configFile&operator=(const configFile &other);
        ~configFile(void);
        std::fstream &getFile();
        class   BadRunException : public std::exception
        {
            const char* what() const throw()
            {
                return ("executable should run as follows: ./webserv [configFile]");
            }
        };
        class   BadFileException : public std::exception
        {
            const char* what() const throw()
            {
                return ("Bad Config File Name");
            }
        };
};

typedef struct routes
{
    std::map<std::string, std::string>  directory, root, index, autoindex,
                                        allowed_methodes, upload, cgi, Return;
    int                                 returnValue;
}   location;

typedef struct info
{
    std::vector<location>               locationsInfo;
    std::map<int, std::string>          errorPages;
    std::map<std::string, std::string>  limitClientBody, port, host, serverName, defaultRoot;
    std::vector<std::string>            others, locations;
}   informations;

class   servers
{
    private:
        std::vector<std::string>            server;
        std::map<int, informations>         serversInfo;
        bool                                rootLocation;
    public:
        servers(void);
        ~servers(void);
        servers(std::fstream &cFile);
        servers(const servers &other);
        servers&operator=(const servers &other);
        class   BadConetent : public std::exception
        {
            const char *what() const throw()
            {
                return ("Bad Config File Content");
            }
        };
        std::map<int, informations> &getMap(void);
        int                         fillInfos(void);
        int                         serverInfos(int i);
        int                         isolateServers(std::string &s);
};


/*-------------- ysabr code start ---------------*/
struct ParsedCGIOutput
{
    std::map<std::string, std::string> headers;
    std::string body;
    std::string filepath;
    int status;
    int check;

    ParsedCGIOutput() : status(200), check(0) {
        headers["content-type"] = "text/html";
    }
};



typedef struct cgiresponse
{
    int     pid;
    std::clock_t    startTime;
    std::string     queries;
    std::string     cookies;
    std::string     method;
    std::string     contentLength, contentType;
    std::string     input, output;
    std::string     script, binary;
    std::string     pathInfo;

    cgiresponse():startTime(0), method( "GET" ) {}

}cgiInfo;
/*-------------- ysabr code end ---------------*/

/*-------------- yachaab code start ---------------*/
typedef struct codeStat
{
    codeStat()
    {
        statMsg[ 200 ] = "OK";
        statMsg[ 201 ] = "Created";
        statMsg[ 204 ] = "No Content";
        statMsg[ 300 ] = "Multiple Choices";
        statMsg[ 301 ] = "Moved Permanently";
        statMsg[ 400 ] = "Bad Request";
        statMsg[ 403 ] = "Forbidden";
        statMsg[ 404 ] = "Not Found";
        statMsg[ 405 ] = "Method Not Allowed";
        statMsg[ 408 ] = "Request Timeout";
        statMsg[ 409 ] = "Conflict";
        statMsg[ 411 ] = "Length Required";
        statMsg[ 413 ] = "Request Entity Too Large";
        statMsg[ 414 ] = "Request-URI Too Long";
        statMsg[ 500 ] = "Internal Server Error";
        statMsg[ 501 ] = "Not Implemented";
    }
    std::map<int, std::string> statMsg;
} code;

/*-------------- ysabr code start ---------------*/
struct ParsedCGIOutput
{
    std::multimap<std::string, std::string> headers;
    std::string body;
    std::string filepath;
    int status;
    int check;

    ParsedCGIOutput() : status(200), check(0) {
        headers.insert(std::make_pair("content-type", "text/html"));
    }
};
/*-------------- ysabr code end ---------------*/

/*-------------- yachaab code start ---------------*/


typedef struct cgiresponse
{
    int     pid;
    std::clock_t    startTime;
    std::string     queries;
    std::string     cookies;
    std::string     method;
    std::string     contentLength, contentType;
    std::string     input, output;
    std::string     script, binary;
    std::string     pathInfo;

    cgiresponse():startTime(0), method( "GET" ) {}

}cgiInfo;

typedef struct clientRequest
{
    std::map<std::string, std::string> headers;
    std::string     fullRequest;
    std::string     remainingBody;
    std::string     extension;
    std::string     partialChunkHeader;
    std::string     filename;
    std::string     scriptName;

    size_t          bytesWrite, contentlength;
    size_t          chunkSizeSum, currentChunkSize;
    size_t          limitClientBodySize;
    int             stat, chunkHeaderStart;

    bool            fetchHeaderDone, processingHeaderDone, processingRequestDone;
    bool            transferEncoding, isContentLength, isChunkHeader, locationGotChecked;
    bool            storeHeader, readyToSendRes, cgi, cgiGET ,expectCRLF;
    cgiInfo         cgiInfo;
    std::ofstream*   bodyStream;


    clientRequest()
    {
        fetchHeaderDone         = false;
        processingHeaderDone    = false;
        transferEncoding        = false;
        isContentLength         = false;
        processingRequestDone   = false;
        storeHeader             = false;
        locationGotChecked      = false;
        readyToSendRes          = false;
        cgi                     = false;
        cgiGET                  = false;
        expectCRLF              = false;
        isChunkHeader           = true;
        bytesWrite              = 0;
        chunkSizeSum            = 0;
        limitClientBodySize     = 0;
        contentlength           = 0;
        stat                    = 0;
        extension               = "";
        // partialChunkHeader      = "";
        bodyStream              = new std::ofstream;
        std::cout << "REQUEST STRUCT CONSTRUCTED" << std::endl;

    }
} Request;
/*-------------- yachaab code end ---------------*/

typedef struct clientResponse
{
    std::ifstream   fileStream;
    std::string     filePath;
    size_t          totalSize;
    size_t          bytesSent;
    std::string     responseHeader;
    int             pid;
    bool            waitCgi;
    informations    info;
    std::clock_t    startTime;
    std::vector<std::string> removeFiles;
    enum Status{
        Pending,
        InProgress,
        Complete
    } status;
    clientResponse();
    ~clientResponse();
    clientResponse(const clientResponse&);
    clientResponse& operator=(const clientResponse&);
    void setResponseHeader(const std::string&);
    ParsedCGIOutput parseCGIOutput(std::string&);
    int sendResponseFromCGI(int clientSocket, ParsedCGIOutput& cgiOutput, struct clientResponse&);
} response;



class Logger
{
public:
    static std::ofstream& log()
    {
        static std::ofstream myfile_logs;
        if ( !myfile_logs.is_open() )
            myfile_logs.open( "./logging/logs", std::ios::out | std::ios::trunc );
        return myfile_logs;
    }
};

class Except : public std::exception
{
private:
    const char* error;

public:
    Except( const char* msg ) : error ( msg ) {}

    virtual const char* what() const throw()
    {
        return ( error );
    }
};

class   connection
{
private:
    std::map<int, int>                            clientsSock; // each client fd with the server fd that he connect with it in it's value
    std::map<int, Request>                        Requests; // each client fd with it's data in the value
    std::map<int, ParsedCGIOutput>                Cgires; // each client fd with it's data in the value
    std::map<int, response>                       Response;
    std::map<int, informations>                   OverLoad; //Here You Will Find The Informations As A Values For The Fds Of The Sockets Servers
    std::map<int, struct sockaddr_in>             serversSock; // each server fd in key with a ready struct on it's value
    std::vector<int>                              responsetEnd, EndFd;
    std::vector<std::map<int, int>::iterator>     exited;
    std::vector<std::map<int, Request>::iterator> requestEnd;
    std::map<std::string, informations>           notBindingServers;
    std::map<int, clock_t>                        clientTimerToSendRequest;
    std::map<int, clock_t>                        clientTimerToEndRequest;
public:
    connection();
    ~connection();
    connection(std::map<int, informations> &infos);
    connection(const connection &other);
    connection&operator=(const connection &other);
    code    codeMsg;
    void    serversEndPoint(std::map<int, informations> &info);
    void    checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it);
    void    checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it, const std::map<int, informations>& );
    void    dropClient( int&, std::map<int, int>::iterator & );
    // void    closeTheExitClients(void);
    void    processingClientRequest( int, char*, Request&, int );
    void    processingBody( Request&, char*, int, int );
    int     location_support_upload( Request& , int );
    void    processingHeader( Request& );
    void    fetchRequestHeader( Request&, char *, int );

    void    handleRequestGET(int, Request&, const informations&);
    void    handleRequestDELETE(int, Request&, const informations&);
    void        serveErrorPage(int, int, const informations&);
};
//pars functions
int         isValidIp4(std::string &value);
int         getContent(std::string &server);
int         normalCheck(std::string &value);
int         checkLocations(informations &tmp);
int         isServer(std::string &s, size_t i);
int         isAgoodServer(std::string &server);
int         checkInformations(informations &tmp);
int         isInteger(std::string &value, char c);
int         multiValues(std::string &key, std::string &values);
void        showInfo(informations &tmp);
void        showInfo2(informations &tmp);
void        etatInitial(informations &tmp);
void        initializeMonitor(struct pollfd &monitor, int fd);
std::string removeWhiteSpaces(std::string &s);
int         redirection(int *status, std::string &val);
int         errorPages(std::string &all, int *status);
/*-------------- yachaab code start ---------------*/
// void        sendResponse( int&, const std::string& );
// std::string creatTemplate( const char*, int& , code&  );
/*-------------- yachaab code end -----------------*/
/*-------------- ysabr code start ---------------*/
std::string to_string(int);
std::string getMimeType(std::string&);
std::string getNextChunk(response&,size_t);
std::string mapUriToFilePath(std::string&, location);
std::string generateDirectoryListing(const std::string&);
bool        hasNextChunk(response&);
bool        fileExists(std::string&);
bool        isDirectory(const std::string&);
bool        isRegularFile(const std::string&);
bool        removeDirectory(const std::string& path);
void        closeFile(response&);
void        sendResponseChunk(int, response&);
void        openFile(response&,const std::string&);
location    findRouteConfig(std::string&, const informations&);
std::string decodeURI(const std::string&);
bool        isPathWithinRoot(std::string&, std::string&);
std::string resolveFilePath(std::string&);
void        sendResponseFromCGI(int, ParsedCGIOutput&);
void        cleanupResponseFiles(std::vector<std::string>&);
/*-------------- ysabr code end ---------------*/
/*CGI*/
std::string GetExtension(std::string &filename);
void cgiFile(cgiInfo&);
#endif