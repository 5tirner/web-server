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
    std::map<std::string, std::string>  allowed_methodes;
    std::map<std::string, std::string>  autoindex;
    std::map<std::string, std::string>  Return;
    std::map<std::string, std::string>  upload;
    std::map<std::string, std::string>  cgi;
}   location;

typedef struct info
{
    //others
    std::vector<std::string>            others;
    std::map<std::string, std::string>  limitClientBody;
    std::map<std::string, std::string>  port;
    std::map<std::string, std::string>  host;
    std::map<std::string, std::string>  serverName;
    std::map<std::string, std::string>  errorPage;
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
        servers(configFile &cFile);
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
        int             isolateServers(std::string &s);
        int             fillInfos(void);
        int             serverInfos(int i);
};

int         isServer(std::string &s, size_t i);
std::string removeWhiteSpaces(std::string &s);
int         isAgoodServer(std::string &server);
int         getContent(std::string &server);
int         checkInformations(informations &tmp);
int         checkLocations(informations &tmp);
void        etatInitial(informations &tmp);
void        showInfo(informations &tmp);
void        showInfo2(informations &tmp);
int         isAgoodValue1(std::string &value);
int         valueCheck(informations &tmp);

#endif