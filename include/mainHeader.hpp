#ifndef MAINHEADER_HPP
# define MAINHEADER_HPP

# include <iostream>
# include <cstddef>
# include <exception>
# include <fstream>
# include <vector>
# include <string>
# include <map>
# include <cstring>
# include <cstdlib>
# include <sstream>
#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <unistd.h>
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
    /*-------------- yachaab code start ---------------*/
    int         stat;
    bool        fetchHeaderDone;
    bool        processingHeaderDone;
    std::string fullRequest;
    std::string remainingBody;
    std::map<std::string, std::string> headers;
    clientRequest ()
    {
        fetchHeaderDone = false;
        processingHeaderDone = false;
    }
    /*-------------- yachaab code end -----------------*/
    std::map<std::string, std::string>  header;
    // std::fstream                        body;
} Request;

class   connection
{
    private:
        std::map<int, struct sockaddr_in>           serversSock; // each server fd in key with a ready struct on it's value
        std::map<int, int>                          clientsSock; // each client fd with the server fd that he connect with it in it's value
        std::map<int, Request>                      Requests; // each client fd with it's data in the value
        //std::vector<std::map<int, int>::iterator>   exited;
    public:
        connection();
        connection(const connection &other);
        connection(std::map<int, informations> &infos);
        connection&operator=(const connection &other);
        ~connection();
        void    serversEndPoint(std::map<int, informations> &info);
        void    checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it);
        void    checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it);
        void    closeTheExitClients(void);
        /*-------------- yachaab code start ---------------*/
        void    fetchRequestHeader( Request&, char* );
        int     processingHeader( Request& );
        code    codeMsg;
        /*-------------- yachaab code end -----------------*/
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
/*-------------- yachaab code end -----------------*/
//multuplexing functions

void    initializeMonitor(struct pollfd &monitor, int fd);

#endif