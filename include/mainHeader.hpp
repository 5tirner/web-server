#ifndef MAINHEADER_HPP
# define MAINHEADER_HPP
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
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <algorithm>
#include <dirent.h>
#include<csignal>
#include <fcntl.h>

#define RES_HEADER	"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n" // added by yachaab
#define OUT( val ) std::cout << val << std::endl;


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
    std::map<std::string, std::string>  directory;
    std::map<std::string, std::string>  root;
    std::map<std::string, std::string>  index;
    std::map<std::string, std::string>  autoindex;
    std::map<std::string, std::string>  allowed_methodes;
    std::map<std::string, std::string>  upload;
    std::map<std::string, std::string>  cgi;
    std::map<std::string, std::string>  Return;
    int                                 returnValue;
}   location;

typedef struct info
{
    //others
    std::vector<std::string>                 others;
    std::map<std::string, std::string>       limitClientBody;
    std::map<std::string, std::string>       port;
    std::map<std::string, std::string>       host;
    std::map<std::string, std::string>       serverName;
    std::map<int, std::vector<std::string> > error_page;
    //locations
    std::vector<std::string>            locations;
    std::vector<location>               locationsInfo;
}   informations;

class   servers
{
    private:
        std::vector<std::string>    server;
        std::map<int, informations> serversInfo;
    public:
        servers(void);
        servers(std::fstream &cFile);
        servers(const servers &other);
        servers&operator=(const servers &other);
        ~servers(void);
        class   BadConetent : public std::exception
        {
            const char *what() const throw()
            {
                return ("Bad Config File Content");
            }
        };
        int                         isolateServers(std::string &s);
        int                         fillInfos(void);
        int                         serverInfos(int i);
        std::map<int, informations> &getMap(void);
};

//for multuplexing
/*-------------- yachaab code start ---------------*/
typedef struct codeStat
{
    std::map<int, std::string> statMsg;
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
        statMsg[ 413 ] = "Request Entity Too Large";
        statMsg[ 414 ] = "Request-URI Too Long";
        statMsg[ 500 ] = "Internal Server Error";
        statMsg[ 501 ] = "Not Implemented";
    }
} code;
/*-------------- yachaab code start ---------------*/

typedef struct clientRequest
{
    clientRequest() {}
    clientRequest( int rd )
    {
        rc                      = rd;
        fetchHeaderDone         = false;
        processingHeaderDone    = false;
        transferEncoding        = false;
        contentLength           = false;
        processingRequestDone   = false;
        storeHeader             = false;
        isChunkHeader           = true;
        content_length          = 0;
        bodyStream              = new std::ofstream;
        fileName                = "./upload/";
        chunkSizeSum            = 0;
        limitClientBodySize     = 0;
    }
    // ~clientRequest() { if ( bodyStream ) delete  bodyStream; }

    std::map<std::string, std::string> headers, queries;
    std::ofstream*  bodyStream;
    std::string     fullRequest;
    std::string     remainingBody;
    std::string     fileName;

    size_t  content_length;
    size_t  requestBodyLength;
    size_t  chunkSizeSum;
    size_t  currentChunkSize;
    size_t  limitClientBodySize;
    int     stat;
    int     chunkHeaderStart;
    int     rc;

    bool    fetchHeaderDone;
    bool    processingHeaderDone;
    bool    isChunkHeader;
    bool    transferEncoding;
    bool    contentLength;

    bool    processingRequestDone;
    bool    storeHeader;
} Request;

typedef struct clientResponse
{
    std::ifstream   fileStream;
    std::string     filePath;
    size_t          totalSize;
    size_t          bytesSent;
    std::string     responseHeader;
    enum Status
    {
        Pending,
        InProgress,
        Complete
    } status;
    clientResponse();
    clientResponse(const clientResponse& other);
    clientResponse& operator=(const clientResponse& other);
    void setResponseHeader(const std::string& header);
} response;


class   connection
{
    private:
        response                                            storeRes;
        std::map<int, response>                             Response;
        std::map<int, informations>                         OverLoad; //Here You Will Find The Informations As A Values For The Fds Of The Sockets Servers
        std::map<int, struct sockaddr_in>                   serversSock; // each server fd in key with a ready struct on it's value
        std::map<int, int>                                  clientsSock; // each client fd with the server fd that he connect with it in it's value
        std::map<int, Request>                              Requests; // each client fd with it's data in the value
        std::vector<std::map<int, int>::iterator>           exited;
        std::vector<std::map<int, Request>::iterator>       requestEnd;
        std::vector<int>      responsetEnd;
        std::vector<int>                                    EndFd;
        bool                                                readyToSendRes;
    public:
        connection();
        connection(std::map<int, informations> &infos);
        connection(const connection &other);
        connection&operator=(const connection &other);
        ~connection();
        void    serversEndPoint(std::map<int, informations> &info);
        void    checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it,  const std::map<int, informations>&  );//!yachaab edit here: add localisation struct
        void    checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it);
        void    closeTheExitClients(void);
        /*-------------- yachaab code start ---------------*/
        void    fetchRequestHeader( Request&, char* );
        int     processingHeader( Request& );
        void    processingBody( Request&, char*, int, const informations& );
        void    dropClient( int&, std::map<int, int>::iterator & );
        code    codeMsg;
        void    handler(int);
        /*-------------- yachaab code end -----------------*/
    
        /*-------------- ysabr code start ---------------*/
        void    handleRequestGET(int, Request&, const informations&);
        void    handleRequestDELETE(int, Request&, const informations&);
        /*-------------- ysabr code end -----------------*/
};



//pars functions
int         isServer(std::string &s, size_t i);
std::string removeWhiteSpaces(std::string &s);
int         isAgoodServer(std::string &server);
int         getContent(std::string &server);
int         checkInformations(informations &tmp);
int         checkLocations(informations &tmp);
void        etatInitial(informations &tmp);
void        showInfo(informations &tmp);
void        showInfo2(informations &tmp);
int         normalCheck(std::string &value);
int         multiValues(std::string &key, std::string &values);
int         isInteger(std::string &value, char c);
int         isValidIp4(std::string &value);
/*-------------- yachaab code start ---------------*/
int         extractMethodAndUri( Request& );
int	        validateUri( const std::string& );
void	    decomposeQueryParameters( const std::string& );
int         validateUriAndExtractQueries( Request& );
int         extractHttpHeaders( Request& );
bool	    examinHeaders( Request&, std::string&, std::string& );
void        lowcase( std::string& );
int	        validateHeadersProcess( Request& );
void        generateRandomFileName( Request& );
long        parseChunkHeader( Request&, std::string& );
bool        chunkedComplete( Request&, std::string& );
void        processChunkedRequestBody( Request&, char*, int&, bool& );
void        processRegularRequestBody( Request&, char*, int&, bool& );
int         location_support_upload( Request&,  const informations& );
int         validateUriAndExtractQueries( Request& rs );
/*-------------- yachaab code end -----------------*/

/*-------------- ysabr code start ---------------*/
location    findRouteConfig(std::string&, const informations&);
void        openFile(response&,const std::string&);
void        closeFile(response&);
bool        hasNextChunk(response&);
bool        isRegularFile(const std::string&);
bool        isDirectory(const std::string&);
std::string generateDirectoryListing(const std::string&);
std::string to_string(int);
std::string getNextChunk(response&,size_t);
std::string getMimeType(std::string&);
std::string mapUriToFilePath(std::string&, location&);
bool        fileExists(std::string&);
bool        removeDirectory(const std::string& path);
void        sendResponseChunk(int, response&);
/*-------------- ysabr code end ---------------*/


//multuplexing functions

void    initializeMonitor(struct pollfd &monitor, int fd);

#endif