#ifndef MAINHEADER_HPP
#define MAINHEADER_HPP
#include <iostream>
#include <exception>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <exception>
#include <stdexcept>
#include <cstdio>
#include <netinet/in.h>
#include <string>
#include <sys/poll.h>
#include <map>
#include <ctime>
// #include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <algorithm>
#include <dirent.h>
#include <csignal>
#include <fcntl.h>
#include <sys/types.h>
#include <climits>

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
    std::vector<location>                       locationsInfo;
    std::map<int, std::vector<std::string> >    error_page;
    std::map<std::string, std::string>          limitClientBody, port, host, serverName;
    std::vector<std::string>                    others, locations;
}   informations;

class   servers
{
    private:
        std::vector<std::string>    server;
        std::map<int, informations> serversInfo;
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

/*-------------- yachaab code start ---------------*/
typedef struct codeStat
{
    codeStat()
    {
        statMsg[ 200 ] = "OK";
        statMsg[ 201 ] = "Created";
        statMsg[ 204 ] = "No Content";
        statMsg[ 301 ] = "Moved Permanently";
        statMsg[ 400 ] = "Bad Request";
        statMsg[ 403 ] = "Forbidden";
        statMsg[ 404 ] = "Not Found";
        statMsg[ 405 ] = "Method Not Allowed";
        statMsg[ 409 ] = "Conflict";
        statMsg[ 411 ] = "Length Required";
        statMsg[ 413 ] = "Request Entity Too Large";
        statMsg[ 414 ] = "Request-URI Too Long";
        statMsg[ 500 ] = "Internal Server Error";
        statMsg[ 501 ] = "Not Implemented";
    }
    std::map<int, std::string> statMsg;
} code;
/*-------------- yachaab code start ---------------*/

typedef struct clientRequest
{
    clientRequest()
    {
        fetchHeaderDone         = false;
        processingHeaderDone    = false;
        transferEncoding        = false;
        contentLength           = false;
        processingRequestDone   = false;
        storeHeader             = false;
        locationGotChecked      = false;
        readyToSendRes          = false;
        iscr                    = false;
        islf                    = false;
        isChunkHeader           = true;
        content_length          = 0;
        chunkSizeSum            = 0;
        limitClientBodySize     = 0;
        extension               = "";
        bodyStream              = new std::ofstream;
    }

    std::map<std::string, std::string> headers, queries;
    std::ofstream*  bodyStream;
    std::string     fullRequest;
    std::string     remainingBody;
    std::string     filename;
    std::string     extension;

    size_t          content_length;
    size_t          requestBodyLength;
    size_t          chunkSizeSum;
    size_t          currentChunkSize;
    size_t          limitClientBodySize;
    int             stat;
    int             chunkHeaderStart;
    int             rc;

    bool            fetchHeaderDone;
    bool            processingHeaderDone;
    bool            isChunkHeader;
    bool            transferEncoding;
    bool            contentLength;
    bool            processingRequestDone;
    bool            storeHeader;
    bool            readyToSendRes;
    bool            locationGotChecked;
    bool            iscr;
    bool            islf;
} Request;

typedef struct clientResponse
{
    std::ifstream   fileStream;
    std::string     filePath;
    size_t          totalSize;
    size_t          bytesSent;
    std::string     responseHeader;
    enum Status{
        Pending,
        InProgress,
        Complete
    } status;
    clientResponse();
    clientResponse(const clientResponse& other);
    clientResponse& operator=(const clientResponse& other);
    void setResponseHeader(const std::string& header);
} response;

class Logger
{
public:
    static std::ofstream& log()
    {
        static std::ofstream myfile_logs;
        if ( !myfile_logs.is_open() )
            myfile_logs.open( "./logs", std::ios::out | std::ios::trunc );
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
    std::map<int, int>                                  clientsSock; // each client fd with the server fd that he connect with it in it's value
    std::map<int, Request>                              Requests; // each client fd with it's data in the value
    std::map<int, response>                             Response;
    std::map<int, informations>                         OverLoad; //Here You Will Find The Informations As A Values For The Fds Of The Sockets Servers
    std::map<int, struct sockaddr_in>                   serversSock; // each server fd in key with a ready struct on it's value
    std::vector<int>                                    responsetEnd, EndFd;
    std::vector<std::map<int, int>::iterator>           exited;
    std::vector<std::map<int, Request>::iterator>       requestEnd;
public:
    connection();
    ~connection();
    connection(std::map<int, informations> &infos);
    connection(const connection &other);
    connection&operator=(const connection &other);
    code    codeMsg;
    void    serversEndPoint(std::map<int, informations> &info);
    void    checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it);
    void    checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it,  const std::map<int, informations>&  );
    void    dropClient( int&, std::map<int, int>::iterator & );
    // void    closeTheExitClients(void);
    /*-------------- yachaab code start ---------------*/
    void    processingClientRequest( int, char*, Request& , const informations& );
    /*-------------- yachaab code end -----------------*/
    /*-------------- ysabr code start ---------------*/
    void    handleRequestGET(int, Request&, const informations&);
    void    handleRequestDELETE(int, Request&, const informations&);
    /*-------------- ysabr code end -----------------*/
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
/*-------------- yachaab code start ---------------*/
void        processingHeader( Request& );
void        sendResponse( int&, const std::string& );
void        fetchRequestHeader( Request&, char *, int );
void        processingBody( Request&, char*, int, const informations& );
std::string creatTemplate( const char*, int& , code&  );
/*-------------- yachaab code end -----------------*/
/*-------------- ysabr code start ---------------*/
std::string to_string(int);
std::string getMimeType(std::string&);
std::string getNextChunk(response&,size_t);
std::string mapUriToFilePath(std::string&, location&);
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
/*-------------- ysabr code end ---------------*/
#endif