this what I add in my code but you don't use routconfig in function executeCgiScript so can you please check if this code work correctly or I must add something and add some environment : #ifndef MAINHEADER_HPP
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

    bool    fetchHeaderDone;
    bool    processingHeaderDone;
    bool    isChunkHeader;
    bool    transferEncoding;
    bool    contentLength;

    bool    processingRequestDone;
    bool    storeHeader;
    bool    readyToSendRes;
    bool    locationGotChecked;
    bool    iscr;
    bool    islf;
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

class   connection
{
    private:
        std::map<int, response>                             Response;
        std::map<int, informations>                         OverLoad; //Here You Will Find The Informations As A Values For The Fds Of The Sockets Servers
        std::map<int, struct sockaddr_in>                   serversSock; // each server fd in key with a ready struct on it's value
        std::map<int, int>                                  clientsSock; // each client fd with the server fd that he connect with it in it's value
        std::map<int, Request>                              Requests; // each client fd with it's data in the value
        std::vector<std::map<int, int>::iterator>           exited;
        std::vector<std::map<int, Request>::iterator>       requestEnd;
        std::vector<int>                                    responsetEnd;
        std::vector<int>                                    EndFd;
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
        void    fetchRequestHeader( Request&, char *, int );
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
int redirection(int *status, std::string &val);

/*-------------- yachaab code start ---------------*/
int         extractMethodAndUri( Request& );
int	        validateUri( const std::string& );
void	    decomposeQueryParameters( const std::string& );
int         validateUriAndExtractQueries( Request& );
int         extractHttpHeaders( Request& );
bool	    examinHeaders( Request&, std::string&, std::string& );
void        lowcase( std::string& );
int	        validateHeadersProcess( Request& );
void        generateRandomFileName( Request&, std::string& );
size_t      parseChunkHeader( Request&, std::string& );
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
int normalCheck(std::string &value)
{
    if (value.size() == 0)
        return (1);
    std::string save;
    size_t i = 0, j = value.size() - 1;
    while ((i < value.size()) && (value[i] == ' ' || value[i] == '\t'))
        i++;
    while ((j) && (value[j] == ' ' || value[j] == '\t'
            || value[j] == '\n' || value[j] == ';' || value[j] == '{'))
        j--;
    while (i <= j)
    {
        save.push_back(value[i]);
        i++;
    }
    //std::cout << value + " Become " + save << std::endl;
    if (save.size() == 0 || save == ";") return (1);
    if (strchr(save.c_str(), ' ') || strchr(save.c_str(), '\t' ))
        return (1);
    value = save;
    return (0);
}

int     methodesSyntax(std::string &value)
{
    std::string check;
    int Post = 0, Get = 0, Delete = 0; 
    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == ' ' || value[i] == '\t')
        {
            //std::cout << "---->" << check << std::endl;
            if (check == "POST") Post++;
            else if (check == "DELETE") Delete++;
            else if (check == "GET") Get++;
            else return (1);
            if (Post > 1 || Get > 1 || Delete > 1)
                return (1);
            check.clear();
        }
        else
            check.push_back(value[i]);
    }
    if (check.size())
    {
        if (check == "POST") Post++;
        else if (check == "DELETE") Delete++;
        else if (check == "GET") Get++;
        else return (1);
        if (Post > 1 || Get > 1 || Delete > 1)
            return (1);
    }
    return (0);
}

int    justMakeItRight(std::string &values)
{
    size_t i = 0, j = values.size() - 1;
    std::string save;
    while ((i < values.size()) && (values[i] == ' ' || values[i] == '\t'))
        i++;
    while ((j > 0) && (values[j] == ' ' || values[j] == '\t'
            || values[j] == ';' || values[j] == '\n'))
            j--;
    while (i <= j)
    {
        save.push_back(values[i]);
        i++;
    }
    if (save == ";" || save.size() == 0)
        return (1);
    values = save;
    return (0);
}

int cgiAndUploadSyntax(std::string &values)
{
    std::string save;
    size_t i = 0;
    int check = 0;
    for (; i < values.size(); i++)
    {
        if ((i != 0) && (values[i] != ' ' && values[i] != '\t'))
        {
            if (values[i - 1] == ' ' || values[i - 1] == '\t')
                check++;
        }
    }
    if (check > 1)
        return (1);
    i = 0;
    for(; (i < values.size()) && (values[i] != ' ' && values[i] != '\t'); i++)
        save.push_back(values[i]);
    if (save == "on")
    {
        while ((i < values.size()) && (values[i] == ' ' || values[i] == '\t'))
            i++;
        values = &values[i];
        if (values.size() == 0)
            return (1);
    }
    else if (save == "off")
        values = "";
    else
        return (1);
    return (0);
}

int isInteger(std::string &value, char c)
{
    if ((c == 'P') && (atoi(value.c_str()) < 1024 || atoi(value.c_str()) > 65535))
        return (1);
    else if (c == 'N' && atoi(value.c_str()) <= 0)
        return (1);
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!isdigit(value[i]))
            return (1);
        if (c == 'P' && i > 4)
            return (1);
    }
    return (0);
}

int isValidIp4(std::string &value)
{
    std::string save;
    int point = 0;
    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == '.')
        {
            if (save.size() == 0)
                return (1);
            point++;
            if (point > 3)
                return (1);
            if (save.size() > 3 || atoi(save.c_str()) > 255)
                return (1);
            if (save.size() > 1 && save[0] == '0')
                return (1);
            save.clear();
        }
        else if (!isdigit(value[i]))
            return (1);
        else
            save.push_back(value[i]);
    }
    if (save.size() == 0)
        return (1);
    else
    {
        if (save.size() > 3 || atoi(save.c_str()) > 255)
                return (1);
        if (save.size() > 1 && save[0] == '0')
                return (1);
    }
    if (point != 3)
        return (1);
    return (0);
}

int multiValues(std::string &key, std::string &values)
{
    if (key == "index" || key == "server_name")
    {
        if (justMakeItRight(values))
            return (1);
    }
    else if (key == "allowed_methodes")
    {
        if (justMakeItRight(values) || methodesSyntax(values))
            return (1);
    }
    else if (key == "cgi")
    {
        if (justMakeItRight(values) || cgiAndUploadSyntax(values))
            return (1);
    }
    else if (key == "upload")
    {
        if (justMakeItRight(values) || cgiAndUploadSyntax(values))
            return (1);
    }
    else if (key == "return")
    {
        if (justMakeItRight(values))
            return (1);
    }
    return (0);
}

int redirection(int *status, std::string &val)
{
    if (justMakeItRight(val))
        return (1);
    std::string save;
    size_t i = 0;
    for (; i < val.size(); i++)
    {
        if (val[i] == ' ' || val[i] == '\t')
            break;
        else if (!std::isdigit(val[i]))
            return (1);
        save.push_back(val[i]);
    }
    if (save.size() < 3 || save.size() > 5)
        return (1);
    for (; i < val.size(); i++)
    {
        if (val[i] != ' ' && val[i] != '\t')
            break;
    }
    val = &val[i];
    if (!val.size())
        return (1);
    *status = std::atoi(save.c_str());
    return (0);
}
configFile::configFile(void) {}

configFile::~configFile(void) {}

configFile::configFile(const configFile &other) {*this = other;}

configFile &configFile::operator=(const configFile &other)
{
    this->fileName = other.fileName;
    return (*this);
}

configFile::configFile(int ac, char **av)
{
    if (ac == 1)
        this->fileName = "server.conf";
    else if (ac == 2)
        this->fileName = av[1];
    else
        throw BadRunException();
    // std::cout << "Config File Name: " + this->fileName << std::endl; 
    this->File.open(this->fileName.c_str(), std::ios_base::in);
    if (!this->File)
        throw BadFileException();
}

std::fstream &configFile::getFile()
{
    return (File);
}

int    getContent(std::string &server)
{
    std::string save;
    bool        start = 0;
    size_t      end = server.size() - 1;
    while (end > 0 && server[end] != '}')
    {
        if (server[end] != ' ' && server[end] != '\t' && server[end] != '\n')
        {
            std::cout << "Wrong Thing In The End Of This Server \n" + server << std::endl;
            return (1);
        }
        end--;
    }
    for (size_t i = 0; i < end - 1; i++)
    {
        if (start == 1)
            save.push_back(server[i]);
        else
        {
            if (server[i] == '{')
            {
                start = 1;
                while (i < server.size() && (server[i] == ' ' || server[i] == '\t'
                    || server[i] == '\n'))
                    i++;
                i++;
            }
        }
    }
    server = save;
    return (0);
}

int isAgoodServer(std::string &server)
{
    int openCollad = 0;
    int closeCollad = 0;
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i] == '{' || server[i] == '}')
        {
            if (server[i] == '{') openCollad++;
            else if (server[i] == '}') closeCollad++;
            i++;
            while (i < server.size() && server[i] != '\n')
            {
                if (server[i] != ' ' && server[i] != '\t')
                {
                    std::cout << "Wrong THing After The Brackets:\n" + server << std::endl;
                    return (1);
                }
                i++;
            }
        }
        if (server[i] == ';')
        {
            i++;
            while (i < server.size() && server[i] != '\n')
            {
                if (server[i] != ' ' && server[i] != '\t' && server[i] != ';')
                {
                    std::cout << "Wrong Thing after ; in this:\n" + server << std::endl;
                    return (1);
                }
                i++;
            }
        }
    }
    if (closeCollad != openCollad)
    {
        std::cout << "Wrong Numbers Of The Collads" << std::endl;
        return (1);
    }
    if (getContent(server))
        return (1);
    return (0);
}

int isServer(std::string &s, size_t i)
{
    std::string check;
    int j = 0;
    for (; i < s.size(); i++)
    {
        if (j == 7)
            break;
        check.push_back(s[i]);
        j++;
    }
    if (check == "server{") return (0);
    else if (check == "server " || check == "server\t" || check == "server\n")
    {
        for (; i < s.size(); i++)
        {
            check.push_back(s[i]);
            if (s[i] == '{')
                break;
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
            {
                std::cout << "Find A Server Keyword But With Some Bad Words" << std::endl;
                return (2);
            }
        }
        if (check == "server{" || check == "server {" || check == "server\t{"
            || check == "server\n{" || check == "server \n{" || check == "server\t\n{"
            || check == "server\t \n{" || check == "server \t\n{")
        {
            //std::cout << "After Check ^ is a server" << std::endl;
            return (0);
        }
    }
    return (1);
}

std::string removeWhiteSpaces(std::string &s)
{
    std::string ret;
    size_t i = 0;
    if (s[i] == ' ' || s[i] == '\t')
    {
        while ((i < s.size()) && (s[i] == ' ' || s[i] == '\t')) i++;
        if (i < s.size() && s[i] == '\n') i++;
    }
    while (i < s.size())
    {
        if (s[i] == ' ' || s[i] == '\t')
        {
            ret.push_back(s[i]);
            while ((i < s.size()) && (s[i] == ' ' || s[i] == '\t'))
                i++;
        }
        if (i < s.size())
        {
            while (i < s.size() && s[i] != ' ' && s[i] != '\t')
            {
                if (s[i] == '#')
                    return (ret + "\n");
                ret.push_back(s[i]);
                i++;
            }
        }
    }
    return (ret);
}

std::map<int, informations> &servers::getMap(void)
{
    return (this->serversInfo);
}

servers::servers(void) {}

servers::~servers(void){}

servers::servers(const servers &other){*this = other;}

servers&servers::operator=(const servers &other)
{
    for (size_t i = 0 ; i < other.server.size(); i++)
        this->server[i] = other.server[i];
    return (*this);
}

int servers::isolateServers(std::string &s)
{
    size_t check = s.find("server");
    if (check != 0)
    {
        std::cout << "Problem In The Top Of The File" << std::endl;
        return (1);
    }
    std::string save;
    for (size_t i = 0; i < s.size(); i++)
    {
        int checker = isServer(s, i);
        if (!checker && i != 0)
        {
            this->server.push_back(save);
            save.clear();
        }
        else if (checker == 2) return (1);
        save.push_back(s[i]);
    }
    if (save.size())
        this->server.push_back(save);
    return (0);
}

servers::servers(std::fstream &cFile)
{
    std::string buffer;
    std::string needed;
    std::string all;
    while (std::getline(cFile, buffer))
    {
        buffer.push_back('\n');
        all += buffer;
        if (buffer[0] == '#' || buffer[0] == '\n')
            continue;
        bool checker = 0;
        if (buffer[0] == ' ' || buffer[0] == '\t')
        {
            for(size_t i = 0; i < buffer.size(); i++)
            {
                if (buffer[i] == ' ' || buffer[i] == '\t')
                {
                    if (i + 1 < buffer.size() && buffer[i + 1] == '#')
                    {
                        checker = 1; break;
                    }
                }
                else
                    break;
            }
        }
        if (!checker)
            needed += removeWhiteSpaces(buffer);
    }
    if (this->isolateServers(needed))
    {
        cFile.close();
        throw BadConetent();
    }
    if (this->fillInfos())
    {
        cFile.close();
        throw BadConetent();
    }
    cFile.close();
}

void    showInfo2(informations &tmp)
{
    for (size_t i = 0; i < tmp.locationsInfo.size(); i++)
    {
        std::cout << "Location Number " << i + 1 << ':' << std::endl;
        std::map<std::string, std::string>::iterator it
        = tmp.locationsInfo[i].directory.begin();
        std::cout << "Location "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].root.begin();
        std::cout << "Root " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].index.begin();
        std::cout << "Index " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].allowed_methodes.begin();
        std::cout << "AllowMethodes "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].autoindex.begin();
        std::cout << "AutoIndex " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].Return.begin();
        std::cout << "Return "<< it->first + " - |" + it->second+"| Redirect It With Status="<<tmp.locationsInfo[i].returnValue << std::endl;
        it = tmp.locationsInfo[i].upload.begin();
        std::cout << "Upload "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].cgi.begin();
        std::cout << "Cgi " << it->first + " - |" + it->second+"|" << std::endl;
    }
}
void    showInfo(informations &tmp)
{
    std::map<std::string, std::string>::iterator it = tmp.port.begin();
    std::cout << "Port " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.host.begin();
    std::cout << "Host " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.serverName.begin();
    std::cout << "ServerName " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.limitClientBody.begin();
    std::cout << "LimitClient " << it->first << " - " << "|"+it->second+"|" << std::endl;
}

void    initialLocation(location &save)
{
    save.directory["location"] = "";
    save.root["root"] = "";
    save.index["index"] = "index.html";
    save.allowed_methodes["allowed_methodes"] = "GET";
    save.autoindex["autoindex"] = "";
    save.Return["return"] = "";
    save.upload["upload"] = "";
    save.cgi["cgi"] = "";
    save.returnValue = -1;
}

void    etatInitial(informations &tmp)
{
    tmp.port["listen"] = "1024";
    tmp.host["host"] = "127.0.0.1";
    tmp.serverName["server_name"] = "defualt";
    tmp.limitClientBody["limit_client_body"] = "100";
}

int checkLocations(informations &tmp)
{
    // std::cout << "I will Check The Location Info" << std::endl;
    for (size_t i = 0; i < tmp.locations.size(); i++)
    {
        std::stringstream input(tmp.locations[i]);
        std::string       buffer;
        location          save;
        initialLocation(save);
        while (std::getline(input, buffer))
        {
            std::string key; size_t j = 0;
            for ( ; j < buffer.size(); j++)
            {
                if (buffer[j] == ' ' || buffer[j] == '\t')
                {
                    while (j < buffer.size()
                        && strchr(" \t", buffer[j]))
                        j++;
                    break ;
                }
                key.push_back(buffer[j]);
            }
            if (key == "location")
            {
                save.directory[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.directory.begin(); 
                if (normalCheck(it->second) || it->second == "{" || (it->second[0] == '.' && (it->second[1] && it->second[1] == '.')))
                { std::cerr << "Invalid `Location` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key != "location" && key != "{"
                && key != "}" && !strchr(&buffer[j], ';'))
            {
                std::cerr << "Can't Find `;` here " + buffer << std::endl;
                return (1);
            }
            else if (key == "root")
            {
                save.root[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.root.begin(); 
                if (normalCheck(it->second))
                { std::cerr << "Invalid `Root` Syntax: " + it->second << std::endl; return (1); }
                struct stat metadata;
                if (stat(it->second.c_str(), &metadata))
                { std::cerr << "Invalid Root Path " + it->second << std::endl; return (1);}
            }
            else if (key == "index")
            {
                save.index[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.index.begin(); 
                if (multiValues(key, it->second))
                { std::cerr << "Invalid `Index` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "allowed_methodes")
            {
                save.allowed_methodes[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.allowed_methodes.begin(); 
                if (multiValues(key, it->second))
                { std::cerr << "Invalid `Methodes` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "autoindex")
            {
                save.autoindex[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.autoindex.begin(); 
                if ((normalCheck(it->second)) || (it->second != "on" && it->second != "off"))
                { std::cerr << "Invalid `AutoIndex` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "return")
            {
                save.Return[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.Return.begin();
                if (redirection(&save.returnValue, it->second))
                { std::cerr << "Invalid `Return` Syntax: " << &buffer[j] << std::endl; return (1);}
            }
            else if (key == "upload")
            {
                save.upload[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.upload.begin(); 
                if (multiValues(key, it->second))
                { std::cerr << "Invalid `Upload` Syntax: " + it->second << std::endl; return (1); }
                struct stat metadata;
                if (stat(it->second.c_str(), &metadata))
                { std::cerr << "Invalid Upload Path " + it->second << std::endl; return (1);}
            }
            else if (key == "cgi")
            {
                save.cgi[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.cgi.begin(); 
                if (multiValues(key, it->second))
                { std::cerr << "Invalid `Cgi` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key != "}" && key != "{")
            { std::cerr << "Weird KeyWord " + key << std::endl; return (1);}
            else if (save.root.at("root") == "")
            { std::cerr << "Invalid Root Syntax, Root Can't Be Empty" << std::endl; return (1);}
        }
        tmp.locationsInfo.push_back(save);
    }
    return (0);
}

int checkInformations(informations &tmp)
{
    // std::cout << "I will Check The info" << std::endl;
    for (size_t i = 0; i < tmp.others.size(); i++)
    {
        std::string key; size_t j = 0;
        for (; j < tmp.others[i].size(); j++)
        {
            if (tmp.others[i][j] == ' ' || tmp.others[i][j] == '\t')
            {
                while (j < tmp.others[i].size()
                    && strchr(" \t", tmp.others[i][j]))
                    j++;
                break ;
            }
            key.push_back(tmp.others[i][j]);
        }
        if (!strchr(&tmp.others[i][j], ';'))
        { std::cerr << "Can't Find `;` Here " + tmp.others[i] << std::endl;  return (1); }
        if (key == "listen")
        {
            tmp.port[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.port.begin(); 
            if (normalCheck(it->second) || isInteger(it->second, 'P'))
            { std::cerr << "Invalid `Port` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "host")
        {
            tmp.host[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.host.begin(); 
            if (normalCheck(it->second) || isValidIp4(it->second))
            { std::cerr << "Invalid `Host` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "server_name")
        {
            tmp.serverName[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.serverName.begin(); 
            if (multiValues(key, it->second))
            { std::cerr << "Invalid `ServerName` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "limit_client_body")
        {
            tmp.limitClientBody[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.limitClientBody.begin(); 
            if (normalCheck(it->second) || isInteger(it->second, 'N'))
            { std::cerr << "Invalid `ClienBody` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "error_page")
        {
        }
        else
        { std::cerr << "Weird KeyWord " + key << std::endl; return (1); }
    }
    return (0);
}

int servers::serverInfos(int i)
{
    informations tmp;
    etatInitial(tmp);
    int          check = 0;
    std::string  save;
    std::stringstream input(this->server[i]);
    std::string buffer;
    while (std::getline(input, buffer))
    {
        if (!std::strncmp(buffer.c_str(), "location", 8))
        {
            check = 1;
            save += buffer + "\n";
        }
        else if (check == 0) tmp.others.push_back(buffer);
        else if (check == 1)
        {
            save += buffer + "\n";
            if (std::strchr(buffer.c_str(), '}'))
            {
                check = 0;
                tmp.locations.push_back(save);
                save.clear();
            }
        }
    }
    if (tmp.locations.size() == 0)
    {
        std::cout << "No Location Found" << std::endl;
        return (1);
    }
    if (checkInformations(tmp) || checkLocations(tmp))
        return (1);
    this->serversInfo[i] = tmp;
    return (0);
}

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        if (isAgoodServer(this->server[i]))
            return (1);
        if (this->serverInfos(i))
            return (1);
    }
    return (0);
}

bool removeDirectory(const std::string& path)
{
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        return false;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string entryName = entry->d_name;
        if (entryName == "." || entryName == "..")
            continue;

        std::string fullPath = path + "/" + entryName;

        if (isDirectory(fullPath))
        {
            if (!removeDirectory(fullPath))
            {
                closedir(dir);
                return false;
            }
        }
        else
        {
            if (std::remove(fullPath.c_str()) != 0) {
                closedir(dir);
                return false;
            }
        }
    }
    closedir(dir);
    return std::remove(path.c_str()) == 0;
}

void connection::handleRequestDELETE(int clientSocket, Request& request,const informations& serverConfig)
{
    location routeConfig = findRouteConfig(request.headers["uri"], serverConfig);

    std::string filePath = mapUriToFilePath(request.headers["uri"], routeConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("DELETE") == std::string::npos)
    {
        // sendErrorResponse(clientSocket, 405, "Method Not Allowed");
        return;
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);

    // Check if the file/directory exists
    if (access(filePath.c_str(), F_OK) == -1)
    {
        // sendErrorResponse(clientSocket, 404, "Not Found");
        return;
    }
    // Check for necessary permissions
    if (access(filePath.c_str(), W_OK) == -1)
    {
        // sendErrorResponse(clientSocket, 403, "Forbidden");
        return;
    }
    // Check if it's a directory and handle accordingly
    if (S_ISDIR(path_stat.st_mode))
    {
        if (!removeDirectory(filePath))
        {
            // sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    else
    { 
        if (std::remove(filePath.c_str()) != 0)
        {
            // sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}
std::string getMimeType(std::string& filePath)
{
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".mp4"] = "video/mp4";
    size_t dotPos = filePath.rfind('.');
    if (dotPos != std::string::npos)
    {
        std::string extension = filePath.substr(dotPos);
        if (mimeTypes.find(extension) != mimeTypes.end())
            return mimeTypes[extension];
    }
    return "text/plain";
}

bool fileExists(std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    return file.good();
}

std::string mapUriToFilePath(std::string& uri, location& routeConfig)
{
    std::string filePath;
    std::map<std::string, std::string>::iterator rootIt = routeConfig.root.find("root");
    if (rootIt != routeConfig.root.end())
    {
        filePath = rootIt->second;
        filePath += "/";
    }
    else
        filePath = "/var/www/html/"; // Default path
    if (uri == "/")
    {
        std::map<std::string, std::string>::iterator indexIt = routeConfig.index.find("index");
        if (indexIt != routeConfig.index.end() && !indexIt->second.empty())
        {
            std::istringstream iss(indexIt->second);
            std::string indexFile;
            while (std::getline(iss, indexFile, ' '))
            {
                std::string fullFilePath = filePath + "/" + indexFile;
                if (fileExists(fullFilePath))
                    return fullFilePath; // Return the first existing file
            }
            }
            return filePath + "/index.html"; // Default index file
    }
    else
    {
        std::map<std::string, std::string>::iterator pos  = routeConfig.directory.find("location");
        // std::cout << "index: " << pos->second << std::endl;
        size_t start = pos->second.length();
        filePath += uri.substr(start);
        // std::cout << "+uri: " << filePath << std::endl;
    }
    return filePath;
}


location findRouteConfig(std::string& uri,const informations& serverConfig)
{
    for (size_t i = 0; i < serverConfig.locationsInfo.size(); ++i)
    {
        location loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::const_iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            const std::string& locPath = it->second;
            if (uri.compare(0, locPath.length(), locPath) == 0)
                return loc; // Found a matching location
        }
    }
    throw std::runtime_error("Route not found for URI: " + uri);
}


bool isRegularFile(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISREG(statbuf.st_mode);
}

bool isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}
std::string generateDirectoryListing(const std::string& path)
{
    DIR *dir;
    struct dirent *ent;
    std::ostringstream html;
    std::cout << "path dire: " << path << std::endl;
    html << "<html><head><title>Index of " << path << "</title></head><body>";
    html << "<h1>Index of " << path << "</h1><hr><pre>";

    dir = opendir(path.c_str());
    if (dir != NULL)
    {
        std::cout << "dir: " << dir << std::endl;
        while ((ent = readdir(dir)) != NULL)
            html << "<a href='" << ent->d_name << "'>" << ent->d_name << "</a><br>";
        closedir(dir);
    }
    else
        html << "Cannot access directory.";
    html << "</pre><hr></body></html>";
    return html.str();
}

std::string to_string(int value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

void connection::handleRequestGET(int clientSocket, Request& request,const informations& serverConfig)
{
    location routeConfig = findRouteConfig(request.headers["uri"], serverConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("GET") == std::string::npos)
    {
        // sendErrorResponse(clientSocket, 404, "Method Not allowed");
        return;
    }
        std::cout << "========================================> from return1\n";

    std::map<std::string, std::string>::iterator it = routeConfig.Return.find("return");
    if ( it != routeConfig.Return.end() && !it->second.empty())
    {
        std::cout << "========================================> from return\n";
        std::string redirectURL = it->second; // URL to redirect to
        int redirectStatus = routeConfig.returnValue; // HTTP status code for redirection
        // Create the response header for redirection
        std::string responseD = "HTTP/1.1 " + to_string(redirectStatus) + " Redirected\r\n";
        responseD += "Location: " + redirectURL + "\r\n";
        responseD += "Content-Length: 0\r\n"; // No content in the body
        responseD += "\r\n"; // End of headers
        // response responseData;
        // responseData.setResponseHeader(responseD);
        // request.storeHeader = true;
        // Response[clientSocket] = responseData;
        // send(clientSocket, responseD.c_str(), responseD.size(), 0);
        return;
    }
    else
    {
        // Determine the file path based on the route configuration
        std::string filePath2 = mapUriToFilePath(request.headers["uri"], routeConfig);
        std::string filePath = filePath2;
        std::cout << "=======>: path: " << filePath << std::endl;
        std::string responseD;
        if (isDirectory(filePath))
        {
            std::vector<location>::const_iterator it = serverConfig.locationsInfo.begin();
            std::string check = request.headers["uri"] + it->index.at("index");
            std::map<std::string, std::string>::iterator autoindexIt = routeConfig.autoindex.find("autoindex");
            if (isRegularFile(check))
            {
                responseD = "HTTP/1.1 301 OK\r\n";
                responseD += "Location: " + check + " \r\n";
            }
            else if (autoindexIt != routeConfig.autoindex.end() && autoindexIt->second == "on")
            {
                std::string directoryContent = generateDirectoryListing(filePath);
                std::cout << directoryContent << std::endl;
                responseD = "HTTP/1.1 200 OK\r\n";
                responseD += "Content-Type: text/html\r\n";
                responseD += "Content-Length: " + to_string(directoryContent.size()) + "\r\n";
                
                responseD += "\r\n";
                responseD += directoryContent;
            }
        }
        else
        {
            responseD = "HTTP/1.1 200 OK\r\n";
            responseD += "Content-Type: " + getMimeType(filePath) + "\r\n";
            // std::cout << "check if this correct" << std::endl;;
            /* -------------- yachaab code start ----------------- */
            // std::fstream file;
            // file.open( "./media/video/morpho.mp4", std::fstream::binary | std::fstream::ate | std::fstream::in );
            // size_t size = file.tellg();
            // file.close();
            // // std::cout << "size: " << size << std::endl;
            // responseD += "Content-Length: " + to_string( size ) + "\r\n";
            // std::cout << "MIME TYPE: " << getMimeType(filePath) << std::endl;
            // std::cout << "content length: " << xnxx << std::endl;
            /*-------------- yachaab code ended -----------------*/
        }
        // if (request.flagRespons == 0)
        // {
            response responseData;

            responseData.filePath = filePath;
            responseData.setResponseHeader(responseD);
            request.storeHeader = true;
            Response[clientSocket] = responseData;
            // request.flagRespons = 1;
        // }
    }
}


void    connection::processingBody( Request& rs, char* buffer, int rc, const informations& infoStruct )
{
	if ( rs.headers.at( "method" ) == "get" || rs.headers.at( "method" ) == "delete" )
		rs.readyToSendRes = true;
	else if ( rs.headers["method"] == "post" )
    {
		if ( rs.locationGotChecked == false && location_support_upload( rs, infoStruct ) == -1 )
			throw std::exception();
		if ( rs.transferEncoding == true )
			processChunkedRequestBody( rs, buffer, rc, rs.readyToSendRes );
		if ( rs.contentLength == true )
		{
			if ( rs.contentLength <= rs.limitClientBodySize )
				processRegularRequestBody( rs, buffer , rc, rs.readyToSendRes );
			else
			{
				rs.stat = 413;
				Logger::log() << "[ Error ] Request Entity Too Large" << std::endl;
				throw std::exception();
			}
		}
    }
}

static std::string& getUrl( std::string& uri )
{
	size_t slash = 0;

	for ( size_t i =  1; i < uri.length(); i++ )
	{
		if ( uri.at( i ) == '/' )
			slash = i;
	}
	if ( slash > 0 )
		uri.resize( slash );
	return uri;
}

int location_support_upload( Request& rs, const informations& infoStruct )
{	
	std::string upload;
	std::string method;
	std::string	location;
	std::string newUri;
	size_t i = 0;
	try
	{
		newUri = getUrl( rs.headers.at("uri") );
		for (; i < infoStruct.locationsInfo.size(); i++ )
		{
			location = infoStruct.locationsInfo.at(i).directory.at( "location" );
			if ( location.length() > 1 && location.at( location.length() - 1 ) == '/' )
				location.resize( location.length() - 1 );
			if ( newUri == location )
				break ;
		}
		if ( newUri == location )
		{
			upload   = infoStruct.locationsInfo.at( i ).upload.at( "upload" );
			method	 = infoStruct.locationsInfo.at( i ).allowed_methodes.at( "allowed_methodes" );
			if ( upload[0] )
			{
				if ( method.find( "POST" ) != std::string::npos )
				{
					if ( access( upload.c_str(), F_OK | W_OK | X_OK ) == 0 )
					{
						DIR* directory = opendir( upload.c_str() );
						if ( directory != NULL )
							closedir( directory );
						else
						{
							Logger::log() << "[ Error ] : Client can't upload in this location directory not found" << "\'" << upload << "\'" << std::endl;
							return ( rs.stat = 403, -1 );
						}
					}
					else
					{
						Logger::log() << "[ Error ] : Client can't upload in this location " << "\'" << upload << "\'" << std::endl;
						return ( rs.stat = 403, -1 );
					}
					rs.limitClientBodySize = std::atol( infoStruct.limitClientBody.at("limit_client_body").c_str() ) * 100000000; //! need the exact amount
					if ( rs.limitClientBodySize == 0 )
					{
						Logger::log() << "[ Error ] : Client body size limit is 0" << std::endl;
						return ( rs.stat = 400, -1 );
					}
					rs.locationGotChecked = true;

					if ( !rs.bodyStream->is_open() )
						generateRandomFileName( rs, upload );
					return ( rs.stat = 201, 0 );
				}
			}
		}	
	}
	catch(const std::exception& e)
	{
		Logger::log() << "[ Error ] : Client can't upload in this location " << "\'" << upload << "\'" << std::endl;
		return ( rs.stat = 403, -1 );
	}
	return ( rs.stat = 403, -1 );
}

void generateRandomFileName( Request& rs, std::string& path )
{
	rs.filename = path;
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );

	try {
		if ( rs.filename.length() > 1 && rs.filename.at( rs.filename.length() - 1 ) != '/' )
			rs.filename += "/";
		for ( int i = 0; i < 25; i++ )
			rs.filename.push_back( CHARACTERS[ rand() % CHARACTERS.length() ] );
		rs.filename += rs.extension;
    	rs.bodyStream->open( rs.filename.c_str(), std::ios::binary | std::ios::trunc );
		if ( !rs.bodyStream->is_open() )
		{
			Logger::log() << "[ Error ] : couldn't open file to store request body" << std::endl;
			throw std::exception();
		}
	} catch (...) {
		rs.stat = 500;
		throw std::exception();
	}
}

size_t    parseChunkHeader( Request& rs, std::string& buffer )
{
	std::string	chunkHead;
	long		size;
	size_t 		crlfPos = 0;

	if ( rs.iscr == true )
	{
		std::cout << "YES WE ENTER HERE" << std::endl;
		buffer = buffer.substr( 2 );
		rs.iscr = false;
	}
	else if ( rs.islf == true )
	{
		std::cout << "YES WE ENTER HERE 1" << std::endl;
		buffer = buffer.substr( 1 );
		rs.islf = false;
	}

	crlfPos = buffer.find( "\r\n" );
	
	if ( crlfPos == std::string::npos )
		return std::string::npos;
	chunkHead = buffer.substr( 0, crlfPos );
	if ( !chunkHead.empty() )
		size = std::strtol( chunkHead.c_str(), NULL, 16 );
	else
	{
		rs.stat = 400;
		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
		throw std::exception();
	}
	if ( size == LONG_MAX || size == LONG_MIN )
	{
		rs.stat = 400;
		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
		throw std::exception();
	}
	else
		rs.currentChunkSize = size;
	buffer = buffer.substr( chunkHead.length() + 2 );
	return ( rs.currentChunkSize );
}

bool    chunkedComplete( Request& rs,  std::string& buffer )
{
	size_t	bufflen ( buffer.length() );
	while ( bufflen != 0 )
	{
		if ( rs.isChunkHeader == true )
		{
			rs.currentChunkSize = parseChunkHeader( rs, buffer );
			if ( rs.currentChunkSize == std::string::npos )
				return false;
			rs.chunkSizeSum += rs.currentChunkSize;
			if ( rs.chunkSizeSum > rs.limitClientBodySize )
			{
				rs.stat = 413;
				Logger::log() << "[ Error ] Client bosy size greater than body size limit" << std::endl;
				throw std::exception();
			}
			if ( rs.currentChunkSize == 0 )
				return true;
			bufflen = buffer.length();
		}
		if ( rs.currentChunkSize > buffer.length() )
		{
			rs.bodyStream->write( buffer.c_str(),  buffer.length() );
			if ( !rs.bodyStream->good() )
			{
				rs.stat = 500;
				Logger::log() << "[ Error ] write body stream failed" << std::endl;
				throw std::exception();
			}
			rs.bodyStream->flush();
			rs.currentChunkSize -= buffer.length();
			rs.isChunkHeader = false;
			return ( false );
		}
		else if ( rs.currentChunkSize < buffer.length() )
		{
			rs.bodyStream->write( buffer.c_str(), rs.currentChunkSize );
			if ( !rs.bodyStream->good() )
			{
				rs.stat = 500;
				Logger::log() << "[ Error ] write body stream failed" << std::endl;
				throw std::exception();
			}
			rs.bodyStream->flush();
			bufflen -= rs.currentChunkSize;
			if ( bufflen > 1 )
			{
				buffer = buffer.substr( rs.currentChunkSize + 2 );
				bufflen -= 2;
			}
			else
			{
				buffer = buffer.substr( rs.currentChunkSize + 1 );
				rs.islf = true;
				bufflen -= 1;
			}
			rs.isChunkHeader = true;
		}
		else if ( rs.currentChunkSize == buffer.length() )
		{
			rs.bodyStream->write( buffer.c_str(), rs.currentChunkSize );
			rs.bodyStream->flush();
			bufflen -= rs.currentChunkSize;
			rs.iscr = true;
			rs.isChunkHeader = true;
			return false;
		}
	}
	return false;
}

void    processChunkedRequestBody( Request& rs, char* buffer, int& rc, bool& sendRes)
{
    if ( !rs.remainingBody.empty() )
    {
        if ( chunkedComplete( rs, rs.remainingBody ) )
        {
			rs.stat = 201;
			sendRes = true;
			Logger::log() << "[ sucess ] body file created" << std::endl;
			throw std::exception();
		}
        rs.remainingBody.clear();
    }
    else
    {
        std::string receivedData( buffer, rc );
        if ( chunkedComplete( rs, receivedData ) )
        {
			rs.stat = 201;
			sendRes = true;
			Logger::log() << "[ sucess ] body file created" << std::endl;
			throw std::exception();
		}
    }
}

void	processRegularRequestBody( Request& rs, char* buffer, int& rc, bool& sendRes )
{
	if ( !rs.remainingBody.empty() )
	{
		rs.bodyStream->write( rs.remainingBody.c_str(),  rs.remainingBody.length() );
		rs.bodyStream->flush();
		rs.content_length += rs.remainingBody.length();
		rs.remainingBody.clear();
	}
	else if ( rc ){
		rs.bodyStream->write( buffer,  rc );
		rs.bodyStream->flush();
		rs.content_length += rc;
	}
	if ( !rs.bodyStream->good() )
	{
		rs.stat = 500;
		Logger::log() << "[ Error ] write body stream failed" << std::endl;
		throw std::exception();
	}
	if ( rs.content_length == rs.requestBodyLength )
	{
		rs.stat = 201;
		sendRes = true;
		Logger::log() << "[ sucess ] body file created" << std::endl;
		return;
	}
	else if ( rs.content_length > rs.requestBodyLength )
	{
		rs.stat = 413;
		Logger::log() << "[ Error ] Request Entity Too Large" << std::endl;
		throw std::exception();
	}
}
connection::connection(void) {}

connection::connection(const connection &other){*this = other;}

connection &connection::operator=(const connection &other)
{
    (void)other;
    return (*this);
}

connection::~connection(void){}

void errorGenerator(std::string err, int fd)
{
    if (fd != -1)
        close(fd);
    std::cerr << err << std::endl;
}

void    connection::serversEndPoint(std::map<int, informations> &info)
{
    std::map<int, informations>::iterator it = info.begin();
    while (it != info.end())
    {
        std::cout << "Server Come With Number: " << it->first << std::endl;
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            errorGenerator("Failed To Create Endpoint", -1);
            it++; continue;
        }
        socklen_t   optval = 1;
        struct sockaddr_in sockInfo;
        sockInfo.sin_port = htons(atoi(it->second.port.at("listen").c_str())),
                                    sockInfo.sin_family = AF_INET, sockInfo.sin_addr.s_addr = 0;
        int bufferAllocation = setsockopt(fd, SOL_SOCKET,
                                            SO_REUSEADDR, &optval, sizeof(optval));
        if (bufferAllocation == -1)
        {
            errorGenerator("Can't Allocate Buffer For A Socket", fd);
            it++; continue;
        }
        int AssignName = bind(fd, (struct sockaddr*)&sockInfo, sizeof(sockInfo));
        if (AssignName == -1)
        {
            errorGenerator("Socket Can't Get A Name To Be Defined On The Network", fd);
            it++; continue;
        }
        int listening = listen(fd, 2147483647);
        if (listening == -1)
        {
            errorGenerator("Can't Turn The Socket To Passive One", fd);
            it++; continue;
        }
        this->serversSock[fd] = sockInfo;
        this->OverLoad[fd] = it->second;
        std::cout << "Socket Ready To Listening For The Port: " << it->second.port.at("listen") << " With Number: " << fd << std::endl;
        it++;
    }
}

void    initializeMonitor(struct pollfd &monitor, int fd)
{
    monitor.fd = fd;
    monitor.events = POLLIN | POLLOUT;
}

void    connection::checkClient(struct pollfd &monitor, std::map<int, int>::iterator &it, const std::map<int, informations>& infoMap) //!yachaab edit here: add localisation vector
{
    if ((monitor.revents & POLLIN))
    {
        // std::cout << "Cleint-Side, An Event Happen Into " << monitor.fd << " Endpoint." << std::endl;
        // std::cout << "This Client Is Rlated With Server Endpoint " << it->second << std::endl;
        char buffer[2048];
        int rd = read(monitor.fd, buffer, 2047);
        if (rd == -1)
        {
            std::cerr << "Error: Failed To Read From " << monitor.fd << " Endpoint." << std::endl;
            dropClient(monitor.fd, it);
        }
        else if (rd == 0)
        {
            /*-------------- yachaab edit start ---------------*/
            dropClient( monitor.fd, it );
            /*-------------- yachaab edit end ---------------*/
        }
        else if (rd)
        {
            buffer[rd] = '\0';
            /*-------------- yachaab code start ---------------*/
            try {
                try
                {
                    this->Requests.at(monitor.fd);
                }
                catch(...)
                {
                    this->Requests[monitor.fd] = Request();
                }
                if ( this->Requests.at(monitor.fd).fetchHeaderDone == false )
                    fetchRequestHeader( this->Requests.at(monitor.fd), buffer, rd );
                if ( this->Requests.at(monitor.fd).fetchHeaderDone == true && this->Requests.at(monitor.fd).processingHeaderDone == false )
                    processingHeader( this->Requests[monitor.fd] );
                if ( this->Requests.at(monitor.fd).processingHeaderDone == true )
                    processingBody( this->Requests.at(monitor.fd), buffer, rd, infoMap.at( it->second ) );
                
            } catch ( ... ) {
                std::cerr << codeMsg.statMsg[this->Requests[monitor.fd].stat] << std::endl;
                this->Requests[monitor.fd].readyToSendRes = true;
            }
            /*-------------- yachaab code end -----------------*/
        }
    }
    else if (monitor.revents & POLLHUP)
    {
        std::cerr << "Error: Client-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
        dropClient(monitor.fd, it);
    }
    else if (monitor.revents & POLLERR)
    {
        std::cerr << "Error: Client-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
        dropClient(monitor.fd, it);
    }
    try
    {
        if ((monitor.revents & POLLOUT) && this->Requests.at(monitor.fd).readyToSendRes)
        {
            try {
                if (!this->Requests.at(monitor.fd).storeHeader)
                {
                    if (this->Requests.at(monitor.fd).headers["method"] == "get")
                        handleRequestGET(monitor.fd, this->Requests[monitor.fd], infoMap.at(it->second));
                    else if (this->Requests.at(monitor.fd).headers["method"] == "delete")
                        handleRequestDELETE(monitor.fd, this->Requests[monitor.fd], infoMap.at(it->second));
                    /*-------------- yachaab code start -----------------*/
                    else if (this->Requests.at(monitor.fd).headers["method"] == "post" )
                        throw std::invalid_argument("salina m3a post response");
                    /*-------------- yachaab code ended -----------------*/
                }

                sendResponseChunk(monitor.fd, Response[monitor.fd]);
                if (Response.at(monitor.fd).status == response::Complete)
                    throw std::exception();
            } 
            catch (...) {
                dropClient(monitor.fd, it);
            }
        }
    }
    catch(...)
    {}
}

void    connection::checkServer(struct pollfd &monitor, std::map<int, struct sockaddr_in>::iterator &it)
{
    if ((monitor.revents & POLLIN))
    {
        std::cout << "Server-Side, An event Comming Into " << monitor.fd << " Endpoint." << std::endl;
        socklen_t   addLen = sizeof(it->second);
        int newClient = accept(monitor.fd, (struct sockaddr *)&it->second, &addLen);
        if (newClient == -1)
            std::cerr << "Error: Filed To Create New EndPoint With Socket " << monitor.fd << std::endl;
        else
        {
            std::cerr << "Clients Those Already Here :" << std::endl;
            for (std::map<int, Request>::iterator it = this->Requests.begin(); it != this->Requests.end(); it++)
                std::cerr << "Clinet Of Fd Number: " << it->first << std::endl;
            std::cout << "New Client Added To Endpoint " << monitor.fd << " With Number " << newClient << '.' << std::endl;
            this->clientsSock[newClient] = monitor.fd;
            this->Response[newClient] = response();
        }
        std::cout << "Number Of Client Now Is: " << this->clientsSock.size() << std::endl;
    }
    else if (monitor.revents & POLLHUP)
        std::cerr << "Error: Server-Side, Connection Destroyed For " << monitor.fd << " Endpoint." << std::endl;
    else if (monitor.revents & POLLERR)
        std::cerr << "Error: Server-Side, Unexpected Error Happen Into " << monitor.fd << " Endpoint." << std::endl;
}

connection::connection(std::map<int, informations> &configData)
{
    this->serversEndPoint(configData);

    while (1)
    {
        struct pollfd monitor[this->serversSock.size() + this->clientsSock.size()];
        std::map<int, int>::iterator it1 = this->clientsSock.begin();
        std::map<int, struct sockaddr_in>::iterator it = this->serversSock.begin();
        size_t i = 0;
        while (it1 != this->clientsSock.end())
        {
            initializeMonitor(monitor[i], it1->first);
            it1++;
            i++;
        }
        while (it != this->serversSock.end())
        {
            initializeMonitor(monitor[i], it->first);
            it++;
            i++;
        }
        int eventChecker = poll(monitor, this->clientsSock.size() + this->serversSock.size(), -1);
        if (eventChecker == -1)
            std::cerr << "Error: Poll Failed To When It's Looking For An Event." << std::endl;
        else if (eventChecker)
        {
            i = 0;
            it1 = this->clientsSock.begin();
            while (it1 != this->clientsSock.end())
            {
                this->checkClient(monitor[i], it1, OverLoad);
                it1++;
                i++;
            }
            for (size_t k = 0; k < this->exited.size(); k++)
                    this->clientsSock.erase(this->exited[k]);
            for (size_t k = 0; k < this->requestEnd.size(); k++)
                    this->Requests.erase(this->requestEnd[k]);
            while (!this->EndFd.empty())
            {
                close(this->EndFd.back());
                this->EndFd.pop_back();
            }            
            for (size_t k = 0; k < this->responsetEnd.size(); k++)
                    this->Response.erase(this->responsetEnd[k]);
            this->exited.clear(), this->requestEnd.clear(); this->responsetEnd.clear();
            it = this->serversSock.begin();
            while (it != this->serversSock.end())
            {
                checkServer(monitor[i], it);
                it++;
                i++;
            }
        }
    }
}
/*-------------- yachaab edit start ---------------*/
void connection::dropClient( int& fd, std::map<int, int>::iterator &it )
{
    if (std::find(this->EndFd.begin(), this->EndFd.end(), fd) != this->EndFd.end())
        return;
    this->EndFd.push_back(fd);
    std::cout << "-> Fd Closed." << std::endl;
    std::cout << "Search For Data..." << std::endl;

    std::map<int, Request>::iterator toRemove = this->Requests.find(it->first);
    std::map<int, response>::iterator rm = this->Response.find(it->first);

    if (rm != this->Response.end())
        this->responsetEnd.push_back(it->first);
    if (toRemove != this->Requests.end())
    {
        std::cout << "Found Some Data For: " << toRemove->first << std::endl;
        this->requestEnd.push_back(toRemove);
        std::cout << "Data Deleted." << std::endl;
    }
    std::cerr << "Client " << it->first << " Related With Server "
    << it->second << " Exited." << std::endl;
    this->exited.push_back(it);
    std::cout << "Number Of Client Left: " << this->clientsSock.size() - 1 << std::endl;
}
/*-------------- yachaab edit start ---------------*/

/*-------------- yachaab code start ---------------*/
void    connection::fetchRequestHeader( Request& rs, char* buffer, int rc )
{
    rs.fullRequest.append( buffer, rc );
	size_t	lfp = rs.fullRequest.find( "\n\n" );
	size_t	crp = rs.fullRequest.find( "\r\n\r\n" );
	if ( lfp != std::string::npos )
	{
		rs.remainingBody = rs.fullRequest.substr( lfp + 2);
     	rs.fetchHeaderDone = true;
	}
	if ( crp != std::string::npos )
	{
		rs.remainingBody = rs.fullRequest.substr( crp + 4 );
     	rs.fetchHeaderDone = true;
	}
}

int connection::processingHeader( Request& rs )
{
    if ( extractMethodAndUri( rs ) == -1 )
		throw std::exception();
	if ( validateUriAndExtractQueries( rs ) == -1 )
		throw std::exception();
	if ( extractHttpHeaders( rs ) == -1)
		throw std::exception();
	if ( validateHeadersProcess( rs ) == -1 )
		throw std::exception();
    return ( 0 );
}

int extractMethodAndUri( Request& rs )
{
	std::string startLine;
    size_t      carriagepos;

	try
	{
		startLine = rs.fullRequest.substr( 0, rs.fullRequest.find( "\n" ) );
        carriagepos = startLine.find("\r");
		if ( carriagepos != std::string::npos )
        	startLine.resize(carriagepos);

		int spNbr = std::count( startLine.begin(), startLine.end(), ' ' );
		if ( spNbr != 2 )
			throw std::exception();

		rs.headers["method"]    =	startLine.substr( 0, startLine.find( ' ' )  );
		rs.headers["uri"]       =	startLine.substr( rs.headers["method"].length() + 1, startLine.find_last_of( ' ' ) -  rs.headers["method"].length() - 1 );
		rs.headers["version"]   =	startLine.substr( rs.headers["method"].length() + rs.headers["uri"].length() + 2 );
		lowcase( rs.headers["method"] );
		lowcase( rs.headers["uri"] );
		lowcase( rs.headers["version"] );

		if ( rs.headers["version"] != "http/1.1" )
			throw std::exception();
		if ( rs.headers["method"] != "get" && rs.headers["method"] != "post" && rs.headers["method"] != "delete" )
			throw std::exception();
	}catch( ... )
	{
		return ( rs.stat = 400, -1 );
	}
	return ( 0 );
}

int	validateUri( const std::string& uri )
{
    if ( uri.empty() )
		return ( -1 );
    if ( uri.at( 0 ) != '/' )
		return ( -1 );
    if ( uri.find(' ') != std::string::npos )
		return ( -1 );
	if ( uri.length() > 2048 )
		return ( -1 );
	return ( 0 );
}

void	decomposeQueryParameters( const std::string& query, Request& rs )
{
    std::string param;
    std::stringstream ss( query );

    while ( std::getline(ss, param, '&') )
	{
        if ( param.empty() ) continue;
        size_t equalPos		=	param.find( '=' );
        std::string key		=	param.substr( 0, equalPos );
        std::string value	=	equalPos != std::string::npos ? param.substr( equalPos + 1 ) : "";
        rs.queries[key] = value;
    }
}

std::string decodeURI(const std::string& uri)
{
    std::string result;
    for (std::size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == '%' && i + 2 < uri.length())
        {
            std::string hex = uri.substr(i + 1, 2);
            std::stringstream ss;
            int ch;
            ss << std::hex << hex;
            ss >> ch;
            result += static_cast<char>(ch);
            i += 2;
        }
        else
            result += uri[i];
    }
    return result;
}

int validateUriAndExtractQueries( Request& rs )
{
	if ( validateUri( rs.headers["uri"] ) == -1 )
	{
		if ( rs.headers["uri"].length() > 2048 )
			return ( rs.stat = 414, -1 );
		return ( rs.stat = 400, -1 );
	}

	rs.headers["uri"] = decodeURI(rs.headers["uri"]);

	size_t queryPos = rs.headers["uri"].find('?');
    if ( queryPos != std::string::npos )
	{
        std::string query = rs.headers["uri"].substr( queryPos + 1 );
        rs.headers["uri"] = rs.headers["uri"].substr( 0, queryPos );
        decomposeQueryParameters( query, rs );
    }
	return ( 0 );
}

void lowcase( std::string& str )
{
	for ( size_t i = 0; i < str.length(); i++  )
	{
		if ( str[ i ] >= 65 && str[ i ] <= 90 )
			str[ i ] += 32;
	}
}

int whiteSpace( char ch )
{
	if ( ch == '\t' || ch == ' ' || ch == '\r' || ch == '\f' || ch == '\n' )
		return 1;
	return 0;
}

void strTrim( std::string& str )
{
	size_t i = 0, j = str.length() - 1;

	for ( ; i < str.length() && whiteSpace( str[i] ); i++ );
	for ( ; j >= 0 && whiteSpace( str[j] ); j-- );
	str = str.substr( i, (j - i) + 1 );
}

bool	examinHeaders( Request& rs, std::string& first, std::string& second )
{
	if ( first == "content-length" )
	{
		rs.contentLength = true;
		rs.requestBodyLength = std::atoi( second.c_str() );
	}
	else if ( first == "transfer-encoding" )
	{
		if ( second == "chunked" )
			rs.transferEncoding = true;
		else
			return ( rs.stat = 501, false ) ;
	}
	if ( rs.transferEncoding == true && rs.contentLength == true )
		rs.contentLength = false;
	if ( rs.transferEncoding == false && rs.contentLength == true )
	{
		if ( rs.requestBodyLength == 0 )
			return ( rs.stat = 400, false );
	}
	if ( first == "host" )
	{
		if ( second.empty() )
			return ( rs.stat = 400, false );
	}
	if ( first == "content-type" )
	{
		if ( second.empty() )
			return ( rs.stat = 400, false );
		std::string s1, s2;
		size_t		slash( second.find( '/' ) );
		if ( slash == std::string::npos )
			return ( rs.stat = 400, false ); // not sure;
		
		s1 = second.substr( 0 , slash );
		s2 = second.substr( slash + 1 );

		if ( s1.empty() || s2.empty() )
			return ( rs.stat = 400, false );
		if ( s1 == "multipart" )
			return ( rs.stat = 501, false );
		else
			rs.extension = "." + s2;
	}
	return true;
}

int	extractHttpHeaders( Request& rs )
{
	std::string									line, first, second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( rs.fullRequest );

	try
	{
		std::getline( headerStream, line );
		while ( std::getline( headerStream, line ) && line != "\r" )
		{
			if ( line.length() == 0 )
				break ;
			header_lines.push_back( line );
		}

		for ( it = header_lines.begin(); it != header_lines.end(); ++it )
		{
			first	=	it->substr( 0, it->find_first_of( ':' ) );
			second	=	it->substr( it->find_first_of( ':' ) + 1 );
			
			lowcase( first );
			lowcase( second );
			strTrim( second );

			if ( !examinHeaders( rs, first, second ) )
				throw std::invalid_argument( "bad request: Invalid header" );
		
			rs.headers[ first ] = second;
		}
		rs.processingHeaderDone = true;
	}
	catch( const std::exception& e )
	{
		return ( rs.stat = 400, -1 );
	}
	return ( 0 );
}

int	validateHeadersProcess( Request& rs )
{
	if ( rs.headers.find( "host" ) == rs.headers.end() )
		return ( rs.stat = 400, -1 );
	if ( rs.headers["method"] == "post" )
	{
		if ( rs.transferEncoding == false )
		{
			if ( rs.contentLength == false )
				return ( rs.stat = 411, -1 );
		}
		else
		{
			if ( rs.contentLength == true )
				rs.contentLength = false;
		}
	}
	return 0;

}

/*-------------- yachaab code end ---------------*/

response::clientResponse() : totalSize(0), bytesSent(0), status(Pending)
{
    filePath = "random";
    responseHeader = "random";
}

response::clientResponse(const clientResponse& other)
{
    filePath = other.filePath;
    totalSize = other.totalSize;
    bytesSent = other.bytesSent;
    status = other.status;
    responseHeader = other.responseHeader;
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
        std::cout << "File Not Opened" << std::endl;
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
        if (respData.fileStream)
            respData.fileStream.close();
        openFile(respData ,respData.filePath); // Ensure to provide file path
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
            std::string lastChunk = "0\r\n\r\n";
            send(clientSocket, lastChunk.c_str(), lastChunk.size(), 0);
            closeFile(respData);
            respData.status = response::Complete;

        }
    }
}
void showmeConfigthings(std::map<int, informations> &general)
{
    std::map<int, informations>::iterator it = general.begin();
    while (it != general.end())
    {
        std::cout << "Server Number " << it->first + 1 << ":" << std::endl;
        std::cout << "About Server:" << std::endl;
        showInfo(it->second);
        std::cout << "About Locations:" << std::endl;
        showInfo2(it->second);
        std::cout << "////////////////////////////////////////////////" << std::endl;
        it++;
    }
}
int main(int ac, char **av, char **env)
{
    signal(SIGPIPE, SIG_IGN);
    (void)env;
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile.getFile());
        // showmeConfigthings(start.getMap());
        connection doMuluplexing(start.getMap());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
}