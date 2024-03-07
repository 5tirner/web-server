

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
#include <sys/types.h> 
#include <sys/wait.h> 

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
        informations getServerInfo(int index)
        {
            if (serversInfo.find(index) != serversInfo.end())
                return serversInfo[index];
            return serversInfo[index];
        }
        int                         isolateServers(std::string &s);
        int                         fillInfos(void);
        int                         serverInfos(int i);
        std::map<int, informations> &getMap(void);
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
int         normalCheck(std::string &value);
int         multiValues(std::string &key, std::string &values);
int         isInteger(std::string &value, char c);
int         isValidIp4(std::string &value);

int normalCheck(std::string &value)
{
    if (value.size() == 0)
        return (0);
    std::string save;
    size_t i = 0, j = value.size() - 1;
    while ((i < value.size()) && (value[i] == ' ' || value[i] == '\t'))
        i++;
    while ((j > 0) && (value[j] == ' ' || value[j] == '\t'
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
        values = "off";
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
            if (point > 2)
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
        this->fileName = "one";
    else if (ac == 2)
        this->fileName = av[1];
    else
        throw BadRunException();
    std::cout << "Config File Name: " + this->fileName << std::endl; 
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
            || check == "server\t \n{" || check == "server \t\n")
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

servers::servers(configFile &cFile)
{
    std::string buffer;
    std::string needed;
    std::string all;
    while (std::getline(cFile.getFile(), buffer))
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
    std::cout << "I will Isolate Servers" << std::endl;
    if (this->isolateServers(needed))
        throw BadConetent();
    std::cout << "I will Fill Infos" << std::endl;
    if (this->fillInfos())
        throw BadConetent();
    std::map<int, informations>::iterator it = this->serversInfo.begin();

    while (it != this->serversInfo.end())
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
        std::cout << "Return "<< it->first + " - |" + it->second+"|" << std::endl;
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
    save.directory["location"] = "No_Location";
    save.root["root"] = "No_root";
    save.index["index"] = "No_thing";
    save.allowed_methodes["allowed_methodes"] = "GET";
    save.autoindex["autoindex"] = "off";
    save.Return["return"] = "0";
    save.upload["upload"] = "off";
    save.cgi["cgi"] = "off";
}

void    etatInitial(informations &tmp)
{
    tmp.port["listen"] = "1025";
    tmp.host["host"] = "127.0.0.1";
    tmp.serverName["server_name"] = "defualt";
    tmp.limitClientBody["limit_client_body"] = "10";
}

int checkLocations(informations &tmp)
{
    std::cout << "I will Check The Location Info" << std::endl;
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
                if (normalCheck(it->second) || it->second == "{")
                { std::cout << "Invalid `Location` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key != "location" && key != "{"
                && key != "}" && !strchr(&buffer[j], ';'))
            {
                std::cout << "Can't Find `;` here " + buffer << std::endl;
                return (1);
            }
            else if (key == "root")
            {
                save.root[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.root.begin(); 
                if (normalCheck(it->second))
                { std::cout << "Invalid `Root` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "index")
            {
                save.index[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.index.begin(); 
                if (multiValues(key, it->second))
                { std::cout << "Invalid `Index` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "allowed_methodes")
            {
                save.allowed_methodes[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.allowed_methodes.begin(); 
                if (multiValues(key, it->second))
                { std::cout << "Invalid `Methodes` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "autoindex")
            {
                save.autoindex[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.autoindex.begin(); 
                if ((normalCheck(it->second)) || (it->second != "on" && it->second != "off"))
                { std::cout << "Invalid `AutoIndex` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "return")
            {
                save.Return[key] = &buffer[j];
                // if (redirection(save.returnValue, &buffer[j]))
                //     return (1);
            }
            else if (key == "upload")
            {
                save.upload[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.upload.begin(); 
                if (multiValues(key, it->second))
                { std::cout << "Invalid `Upload` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key == "cgi")
            {
                save.cgi[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.cgi.begin(); 
                if (multiValues(key, it->second))
                { std::cout << "Invalid `Cgi` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key != "}" && key != "{")
            {
                std::cout << "Weird KeyWord " + key << std::endl;
                return (1);
            }
        }
        tmp.locationsInfo.push_back(save);
    }
    return (0);
}

int checkInformations(informations &tmp)
{
    std::cout << "I will Check The info" << std::endl;
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
        { std::cout << "Can't Find `;` Here " + tmp.others[i] << std::endl;  return (1); }
        if (key == "listen")
        {
            tmp.port[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.port.begin(); 
            if (normalCheck(it->second) || isInteger(it->second, 'P'))
            { std::cout << "Invalid `Port` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "host")
        {
            tmp.host[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.host.begin(); 
            if (normalCheck(it->second) || isValidIp4(it->second))
            { std::cout << "Invalid `Host` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "server_name")
        {
            tmp.serverName[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.serverName.begin(); 
            if (multiValues(key, it->second))
            { std::cout << "Invalid `ServerName` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "limit_client_body")
        {
            tmp.limitClientBody[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.limitClientBody.begin(); 
            if (normalCheck(it->second) || isInteger(it->second, 'N'))
            { std::cout << "Invalid `ClienBody` Syntax: " + it->second << std::endl; return (1); }
        }
        else if (key == "error_page")
        {
        }
        else
        { std::cout << "Weird KeyWord " + key << std::endl; return (1); }
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
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 1000


class HTTPRequest
{
    public:
        int flagRespons;
        std::string method;
        std::string uri;
        std::string httpVersion;
        std::map<std::string, std::vector<std::string> > headers;
        std::string body;
        std::map<std::string, std::string> queryParams;
        std::string rawRequest;
        bool headersComplete;
        int flag;
        bool respense;
        void parse();
        void clear();
        bool isComplete();
        void appendData(const char* buffer, int length);
        HTTPRequest();
        std::string decodeURI(const std::string& uri);
        void readHeaders(int clientSocket, HTTPRequest& request);
    std::string getFullRequest() const;
};

class ResponseData
{
public:

    std::ifstream fileStream;
    std::string filePath;
    size_t totalSize;
    size_t bytesSent;
    std::string responseHeader;
    enum Status { Pending, InProgress, Complete } status;

    ResponseData() : totalSize(0), bytesSent(0), status(Pending) {
        // std::cout << "I'm constructor\n";
    }

    ResponseData(const ResponseData& other) {
        filePath = other.filePath;
        totalSize = other.totalSize;
        bytesSent = other.bytesSent;
        status = other.status;
        responseHeader = other.responseHeader;
        // if (other.fileStream.is_open())
        //     openFile(filePath);
    }

    ResponseData& operator=(const ResponseData& other)
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
    
    void openFile(const std::string& path)
    {
        // closeFile(); // Ensure any previously opened file is closed
        filePath = path;
        fileStream.open(path, std::ifstream::binary);
        if (!fileStream)
        {
            std::cout << "wa9ila dkhal l 404\n";
            // sendErrorResponse(clientSocket, 404, "Not Found");
            // return;
            status = Complete; // Mark as complete if file open failed
            return;
        }

        fileStream.seekg(0, std::ios::end);
        totalSize = fileStream.tellg();
        fileStream.seekg(0, std::ios::beg);
        status = InProgress;
    }
     void setResponseHeader(const std::string& header)
    {
        responseHeader = header;
    }
    // std::string decimalToHex(size_t decimalValue)
    // {
    //      const char* hexChars = "0123456789ABCDEF";
    //      std::string hexStr;
    //      while (decimalValue > 0)
    //      {
    //          hexStr = hexChars[decimalValue % 16] + hexStr;
    //         decimalValue /= 16;
    //      }
    //      return hexStr.empty() ? "0" : hexStr;
    //  }
    std::string getNextChunk(size_t chunkSize)
    {
        if (!fileStream.is_open() || status != InProgress)
            return "";
        char buffer[chunkSize];
        fileStream.read(buffer, chunkSize);
        size_t bytesRead = fileStream.gcount();
        bytesSent += bytesRead;

        if (bytesRead <  chunkSize || fileStream.eof())
        {
            status = Complete;
            fileStream.close();
        }
        std::string buf(buffer, 2048);
        // std::cout << "buf: " << buf << std::endl;
        // std::stringstream chunk;
        // chunk << std::hex << bytesRead << "\r\n";
        // chunk.write(buffer, bytesRead);
        // chunk << "\r\n";
        // std::cout << chunk.str() << std::endl;
        return buf;
    }

    // void getNextChunk(size_t chunkSize, int clientSocket)
    // {
    //     if (!fileStream.is_open() || status != InProgress)
    //         return ;
    //     char buffer[chunkSize];
    //     fileStream.read(buffer, chunkSize);
    //     size_t bytesRead = fileStream.gcount();
    //     // std::cout <<  << std::endl;
    //     std::cout << bytesRead << std::endl;
    //     bytesSent += bytesRead;

    //     if (bytesRead <  chunkSize || fileStream.eof())
    //     {
    //         status = Complete;
    //         fileStream.close();
    //     }
    //     // std::stringstream ss;
    //     // ss << std::hex << fileStream.gcount();
    //     // // std::cout << ss.str().c_str() << std::endl;
    //     // write(clientSocket, ss.str().c_str(), ss.str().size());
    //     // write(clientSocket, "\r\n", 2);
    //     // write(clientSocket, buffer, fileStream.gcount());
    //     // write(clientSocket, "\r\n", 2);

    //     if(bytesRead == chunkSize)
    //     {
            
    //          buffer[bytesRead - 1] = 0;
    //         //  std::cout << bytesRead << std::endl;
    //          //std::cout << buffer << std::endl;
    //         //  std::string buffer_is_red(buffer);
    //         // std::cout << sizeof()
    //         size_t siz = write(clientSocket, buffer, bytesRead);
    //         // write(clientSocket, "\r\n", 2);
    //         // if (siz < bytesRead)
    //         // {
    //         //     size_t ss = 0;
    //         //     while(1)
    //         //     {    
    //         //         ss = write(clientSocket, buffer,  2048);
    //         //         siz += ss;
    //         //         if (siz >= bytesRead)
    //         //             break;
    //         //     }   
    //         // }
    //         std::cout << "strlen: "<<  strlen(buffer) << std::endl;
    //         memset(buffer, 0, sizeof(buffer));
    //         // if (siz < bytesRead)
    //         //     siz = write(clientSocket, buffer, strlen(buffer));
    //         std::cout << siz << std::endl;
    //         // return buffer_is_red;
    //     }
    //     else
    //         std::cout<< "hhhhhhhhhhhhhhhhh\n";
    //     // std::stringstream chunk;
    //     // chunk << std::hex << bytesRead << "\r\n";
    //     // chunk.write(buffer, bytesRead);
    //     // chunk << "\r\n";
    //     // return chunk.str();
    // }

    bool hasNextChunk()
    {
        if (fileStream.is_open())
        {
            if (bytesSent < totalSize)
                return true;
            else
                return false;
        }
        else
            return false;
    }

    void closeFile()
    {
        if (fileStream.is_open())
            fileStream.close();
        status = Complete;
    }

};

class Server
{
    private:
        std::string serverName;
        int sockfd;
        int client_socket[MAX_CLIENTS];
        std::map<int, HTTPRequest> requests;
        informations serverConfig;
        int epoll_fd;
        static const int MAX_EVENTS = 10;
        std::string      port;
        std::string      host;
        std::map<int, ResponseData> clientResponses;
        int flagstream;
    public:
        int numberServers;
        void initializeEpoll();
        void initializeServer(); // New method to initialize server
        void setupEpoll();
        void run();
        bool isRegularFile(const std::string& path);
        Server();
        Server(int exitSocket,informations &config ,std::string& port, std::string& host);
        void createSocket();
        void bindSocket();
        void listenToSocket();
        void handleConnections();
        void readFileContent(int fd, const std::string& filePath);
        void handleRequestGET( int clientSocket,  HTTPRequest& request,  informations& serverConfig);
        std::string getMimeType(std::string& filePath);
        void handleRequestPOST(int clientSocket,  HTTPRequest& request);   
        void handleRequestDELETE(int clientSocket,  HTTPRequest& request,  informations& serverConfig);  
        bool fileExists(std::string& filePath);
        std::string mapUriToFilePath( std::string& uri, location& routeConfig);
        location findRouteConfig(std::string& uri, informations& serverConfig);
        void sendErrorResponse(int clientSocket, int errorCode,const std::string& errorMessage);
        void setConfig(const informations& config);
        void acceptNewConnection();
        void handleExistingConnections(fd_set& read_fds);
        void initializeFileDescriptorSet(fd_set& read_fds, int& max_sd);
        void processRequest(int clientSocket, HTTPRequest& request);
        bool isDirectory(const std::string& path);
        bool isRequestForThisServer(HTTPRequest& request, std::string& serverName);
        std::string generateDirectoryListing(const std::string& path);
        void handleCGIRequest(int clientSocket, HTTPRequest& request, informations& serverConfig);
        std::string getScriptPathFromURI(const std::string& uri);
    void sendResponseChunk(int clientSocket, ResponseData& respData);
};

#include<set>

Server::Server()
{
    flagstream = 0;
    memset(client_socket, 0, sizeof(client_socket));
}

Server::Server(int exitSocket, informations &config , std::string& port, std::string& host)
: sockfd(exitSocket), flagstream(0)
{
    this->port = port;
    this->host = host;
    this->serverConfig = config;
    memset(client_socket, 0, sizeof(client_socket));
}


void Server::setConfig(const informations& config)
{
    serverConfig = config;
}
void log(const std::string& message)
{
    std::cout << message << std::endl;
}

void exitWithError(const std::string& errorMessage)
{
    log("ERROR: " + errorMessage);
    exit(1);
}




int createAndBindSocket(const std::string& port, const std::string& host)
{
    (void)host;
    int sockfd;
    struct sockaddr_in serverAddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    // Set socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0)
    {
        perror("ERROR getting socket flags");
        exit(1);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("ERROR setting socket to non-blocking");
        exit(1);
    }
    // Enable SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("ERROR setting SO_REUSEADDR");
        close(sockfd);
        exit(1);
    }
    // Bind socket
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = 0;
    serverAddr.sin_port = htons(atoi(port.c_str()));
    if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("ERROR on binding");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}

void Server::listenToSocket()
{
    if (listen(sockfd, SOMAXCONN) < 0)
    {
        perror("ERROR on listen");
        exit(1);
    }
}

bool readLine(std::istringstream& requestStream, std::string& line)
{
    std::getline(requestStream, line);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    return requestStream.good();
}

HTTPRequest::HTTPRequest() : method(""), uri(""), httpVersion(""), body(""), rawRequest(""), headersComplete(false), flag(0), respense(false) 
{
    flagRespons = 0;
}

void toLowerCase(std::string& s)
{
    for (size_t i = 0; i < s.length(); ++i)
        s[i] = std::tolower(s[i]);
}

void HTTPRequest::appendData(const char* buffer, int length)
{
    this->rawRequest.append(buffer, length);
}

bool HTTPRequest::isComplete()
{
    // Check if the end of the headers section is found
    size_t headerEnd = this->rawRequest.find("\r\n\r\n");
    size_t headerEnd2 = this->rawRequest.find("\n\n");
    if (headerEnd == std::string::npos || headerEnd2 == std::string::npos)
        return false; // The end of headers not yet received
    return true; // Headers and body (if applicable) are fully received
}


void HTTPRequest::clear()
{
    // Clear all the member variables to thei0r default state
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








struct FormDataPart
{
    std::map<std::string, std::string> headers;
    std::string body;
};

struct MultipartFormData
{
    std::vector<FormDataPart> parts;
};

std::map<std::string, std::string> parseUrlEncoded(const std::string& body)
{
    // std::cout << "body2: " << body << std::endl;
    std::map<std::string, std::string> data;
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&'))
    {
        // std::cout << "pair: " << pair << std::endl;
        size_t delimiter_pos = pair.find('=');
        if (delimiter_pos != std::string::npos)
        {
            std::string key = pair.substr(0, delimiter_pos);
            std::string value = pair.substr(delimiter_pos + 1);
            // std::cout << "key: " << key << " value: " << value << std::endl;
            data[key] = value;
        }
    }
    return data;
}


MultipartFormData parseMultipartFormData(const std::string& body, const std::string& boundary)
{
    MultipartFormData formData;
    std::string delimiter = "--" + boundary + "\r\n";
    std::string endDelimiter = "--" + boundary + "--";

    size_t pos = 0;
    size_t nextPos;

    while ((nextPos = body.find(delimiter, pos)) != std::string::npos) {
        size_t partEnd = body.find(delimiter, nextPos + delimiter.size());
        if (partEnd == std::string::npos) {
            partEnd = body.find(endDelimiter, nextPos + delimiter.size());
            if (partEnd == std::string::npos) break;  // End of data
        }

        std::string part = body.substr(nextPos + delimiter.size(), partEnd - nextPos - delimiter.size());

        // Parse headers and body of each part
        size_t headerEnd = part.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            FormDataPart formDataPart;

            std::string headersPart = part.substr(0, headerEnd);
            std::istringstream headerStream(headersPart);
            std::string headerLine;

            while (std::getline(headerStream, headerLine) && !headerLine.empty()) {
                size_t colonPos = headerLine.find(":");
                if (colonPos != std::string::npos) {
                    std::string key = headerLine.substr(0, colonPos);
                    std::string value = headerLine.substr(colonPos + 2, headerLine.length() - colonPos - 3); // -3 for \r\n
                    formDataPart.headers[key] = value;
                }
            }

            formDataPart.body = part.substr(headerEnd + 4); // +4 to skip \r\n\r\n
            formData.parts.push_back(formDataPart);
        }

        pos = partEnd + delimiter.size();
    }

    return formData;
}


typedef std::map<std::string, std::string> JsonData;

JsonData parseJson(const std::string& json)
{
    JsonData data;
    size_t pos = 0;

    // Remove whitespace
    std::string str;
    for (size_t i = 0; i < json.length(); ++i) {
        if (json[i] != ' ' && json[i] != '\n' && json[i] != '\t' && json[i] != '\r') {
            str += json[i];
        }
    }

    // Check if it starts and ends with curly braces
    if (str[0] != '{' || str[str.length() - 1] != '}') {
        throw std::runtime_error("Invalid JSON format");
    }

    // Remove the starting and ending curly braces
    str = str.substr(1, str.length() - 2);

    while (pos < str.length()) {
        // Find key
        size_t start = str.find('"', pos) + 1;
        size_t end = str.find('"', start);
        std::string key = str.substr(start, end - start);

        // Find value
        start = str.find('"', end + 1) + 1;
        end = str.find('"', start);
        std::string value = str.substr(start, end - start);

        // Add to map
        data[key] = value;

        pos = end + 1;
    }

    return data;
}




void Server::handleRequestPOST(int clientSocket, HTTPRequest& request)
{
    // Check Content-Type header
    std::string contentType = request.headers["content-type"].front();
    // std::cout << "POST contentType: " << contentType << std::endl;

    if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
    {
        // std::cout << "Content dkhal l application/x-www-form-urlencoded\n";
        // std::cout << "body: " << request.body << std::endl;
        std::map<std::string, std::string> formData = parseUrlEncoded(request.body);
    }
    else if (contentType.find("multipart/form-data") != std::string::npos)
    {
        // Extract boundary
        size_t boundaryPos = contentType.find("boundary=");
        if (boundaryPos == std::string::npos)
        {
            sendErrorResponse(clientSocket, 400, "Bad Request: No boundary in multipart/form-data");
            return;
        }
        std::string boundary = contentType.substr(boundaryPos + 9); // 9 is the length of "boundary="

        // Handle file uploads and multipart data
        MultipartFormData multipartData = parseMultipartFormData(request.body, boundary);
        // Handle multipart data
    }
    else if (contentType.find("application/json") != std::string::npos)
    {
        // Parse JSON data
        JsonData jsonData = parseJson(request.body);
        // Handle JSON data
    }
    else
    {
        // Unsupported Content-Type
        sendErrorResponse(clientSocket, 415, "Unsupported Media Type");
        return;
    }

    // Process the data as needed

    // Send a response to the client
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}


bool Server::fileExists(std::string& filePath)
{
    std::ifstream file(filePath.c_str());
    return file.good();
}





void Server::sendErrorResponse(int clientSocket, int errorCode,const std::string& errorMessage)
{
    std::string response = "HTTP/1.1 " + to_string(errorCode) + " " + errorMessage + "\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Content-Length: " + to_string(errorMessage.size()) + "\r\n";
    response += "\r\n";
    response += errorMessage;
    send(clientSocket, response.c_str(), response.size(), 0);
}



std::string Server::getMimeType(std::string& filePath)
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

std::string Server::mapUriToFilePath(std::string& uri, location& routeConfig)
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


location Server::findRouteConfig(std::string& uri,  informations& serverConfig)
{
    for (size_t i = 0; i < serverConfig.locationsInfo.size(); ++i)
    {
        location& loc = serverConfig.locationsInfo[i];
        std::map<std::string, std::string>::iterator it = loc.directory.find("location");
        if (it != loc.directory.end())
        {
            std::string& locPath = it->second;
            if (uri.compare(0, locPath.length(), locPath) == 0)
                return loc; // Found a matching location
        }
    }
    throw std::runtime_error("Route not found for URI: " + uri);
}
#include <dirent.h>
#include <sstream>

bool Server::isRegularFile(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISREG(statbuf.st_mode);
}

bool Server::isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}
std::string Server::generateDirectoryListing(const std::string& path)
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

bool isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

std::string to_string(int value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

void Server::handleRequestGET(int clientSocket, HTTPRequest& request, informations& serverConfig)
{

    location routeConfig = findRouteConfig(request.uri, serverConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("GET") == std::string::npos)
    {
        sendErrorResponse(clientSocket, 404, "Method Not allowed");
        return;
    }
    // Determine the file path based on the route configuration
    std::string filePath2 = mapUriToFilePath(request.uri, routeConfig);

    std::string filePath = filePath2;
    std::string response;
    if (isDirectory(filePath))
    {
        std::vector<location>::iterator it = serverConfig.locationsInfo.begin();
        std::string check = request.uri + it->index["index"];
        std::map<std::string, std::string>::iterator autoindexIt = routeConfig.autoindex.find("autoindex");
        if (isRegularFile(check))
        {
            response = "HTTP/1.1 301 OK\r\n";
            response += "Location: " + check + " \r\n";
        }
        else if (autoindexIt != routeConfig.autoindex.end() && autoindexIt->second == "on")
        {
            std::string directoryContent = generateDirectoryListing(filePath);
            std::cout << directoryContent << std::endl;
            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + to_string(directoryContent.size()) + "\r\n";
            response += "\r\n";
            response += directoryContent;
         }
    }
    else
    {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + getMimeType(filePath) + "\r\n";
    }
    if (request.flagRespons == 0)
    {
        ResponseData responseData;
        responseData.filePath = filePath;
        responseData.setResponseHeader(response);
        clientResponses[clientSocket] = responseData;
        request.flagRespons = 1;
    }
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

void Server::handleRequestDELETE(int clientSocket, HTTPRequest& request, informations& serverConfig)
{
    location routeConfig = findRouteConfig(request.uri, serverConfig);

    std::string filePath = mapUriToFilePath(request.uri, routeConfig);
    if (routeConfig.allowed_methodes["allowed_methodes"].find("DELETE") == std::string::npos)
    {
        sendErrorResponse(clientSocket, 405, "Method Not Allowed");
        return;
    }

    struct stat path_stat;
    stat(filePath.c_str(), &path_stat);

    // Check if the file/directory exists
    if (access(filePath.c_str(), F_OK) == -1)
    {
        sendErrorResponse(clientSocket, 404, "Not Found");
        return;
    }
    // Check for necessary permissions
    if (access(filePath.c_str(), W_OK) == -1)
    {
        sendErrorResponse(clientSocket, 403, "Forbidden");
        return;
    }
    // Check if it's a directory and handle accordingly
    if (S_ISDIR(path_stat.st_mode))
    {
        if (!removeDirectory(filePath))
        {
            sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    else
    { 
        if (std::remove(filePath.c_str()) != 0)
        {
            sendErrorResponse(clientSocket, 500, "Internal Server Error");
            return;
        }
    }
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, response.c_str(), response.size(), 0);
}

void setNonBlocking(int sock)
{
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl F_GETFL");
		exit(EXIT_FAILURE);
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl F_SETFL , O_NONBLCOK");
		exit(EXIT_FAILURE);
	}
}

#include<csignal>
void Server::initializeEpoll()
{
    epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN; // We're interested in read events
    ev.data.fd = sockfd; // sockfd is your server's listening socket

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
}

std::string HTTPRequest::decodeURI(const std::string& uri)
{
    std::string result;
    for (std::size_t i = 0; i < uri.length(); ++i)
    {
        if (uri[i] == '%' && i + 2 < uri.length())
        {
            std::string hex = uri.substr(i + 1, 2);
            std::cout << "hex: " << hex << std::endl;
            std::stringstream ss;
            int ch;
            ss << std::hex << hex;
            ss >> ch;
            std::cout << "ch: " << ch << std::endl;
            result += static_cast<char>(ch);
            std::cout << "result: " << result << std::endl;
            i += 2;
        }
        else
            result += uri[i];
    }
    return result;
}

void HTTPRequest::readHeaders(int clientSocket, HTTPRequest& request)
{
    char buffer[1024];
    ssize_t bytesRead;

    // Read from the socket
    bytesRead = read(clientSocket, buffer, 1024);
    rawRequest.append(buffer, bytesRead);
    std::istringstream requestStream(request.rawRequest);
    std::string line;
    if (flag == 0)
    {
        if (!readLine(requestStream, line))
            throw std::runtime_error("Wrong request line");
        std::istringstream requestLineStream(line);
        if (!(requestLineStream >> method >> uri >> httpVersion))
            throw std::runtime_error("Wrong request line");
        if (method != "GET" && method != "POST" && method != "DELETE")
            throw std::runtime_error("Unsupported HTTP method");
        if (httpVersion != "HTTP/1.1")
            throw std::runtime_error("Unsupported HTTP Version");

        size_t queryPos = uri.find('?');
        if (queryPos != std::string::npos)
        {
            std::string queryString = uri.substr(queryPos + 1);
            uri = uri.substr(0, queryPos);
            std::istringstream queryStream(queryString);
            std::string param;
            while (std::getline(queryStream, param, '&'))
            {
                size_t equalPos = param.find('=');
                if (equalPos != std::string::npos)
                    queryParams[param.substr(0, equalPos)] = param.substr(equalPos + 1);
            }
        }

        uri = decodeURI(uri);
        flag = 1;
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
    if (headers.find("host") == headers.end() || headers["host"].empty())
        throw std::runtime_error("Host header is missing");
    if (headers.find("content-length") != headers.end() && headers["content-length"].empty())
        throw std::runtime_error("Content-Length header is missing for POST request");
    if ((headers.find("transfer-encoding") != headers.end() && headers["transfer-encoding"][0] != "chunked"))
        throw std::runtime_error("transfer-encoding header is missing for POST request");
    if (rawRequest.find("\r\n\r\n") != std::string::npos || rawRequest.find("\n\n") != std::string::npos)
        headersComplete = true;
    if ( rawRequest.find("\r\n\r\n") != std::string::npos )
        remainingBody = rawRequest.substr( rawRequest.find( "\r\n\r\n" ) + 4 );
}



void Server::readFileContent(int fd, const std::string& filePath)
{
    //flag 0 open fie/flag 1
    std::ifstream file;
    if (flagstream == 0)
    {
        // file(filePath.c_str());
        file.open(filePath.c_str());
        // std::cout << filePath << std::endl;
        if (!file)
            exitWithError("Error: Unable to open file.");
        flagstream = 1;
    }
    char p[1024];
    file.read(p, 1024);

    // while (file.gcount())
    // {
        std::stringstream ss;
        ss << std::hex << file.gcount();
        // std::cout << ss.str().c_str() << std::endl;
        write(fd, ss.str().c_str(), ss.str().size());
        write(fd, "\r\n", 2);
        write(fd, p, file.gcount());
        write(fd, "\r\n", 2);

        file.read(p, 1024);
    // }
    write(fd, "0\r\n\r\n", 5);
    file.close();
}

    // 1024 + 1024 + 4 + 1028  = 3070;
    // 1024 1024 = 2052

void Server::sendResponseChunk(int clientSocket, ResponseData& respData)
{
    if (respData.status == ResponseData::Complete)
        return;

    // Send the response header if needed
    if (respData.status == ResponseData::Pending)
    {
        respData.openFile(respData.filePath); // Ensure to provide file path
        
        respData.responseHeader += "Content-Length: " + std::to_string(respData.totalSize) + "\r\n\r\n";
        send(clientSocket, respData.responseHeader.c_str(), respData.responseHeader.size(), 0);
        respData.status = ResponseData::InProgress;
    }
    else if (respData.status == ResponseData::InProgress)
    {
        // Assuming getNextChunk is a method that reads the next part of the file
        //     send(clientSocket, chunk.c_str(), chunk.size(), 0);
        std::string chunk = respData.getNextChunk(2048); 
        // std::cout << chunk << std::endl;
        if (!chunk.empty())
            send(clientSocket, chunk.c_str(), chunk.size(), 0);

        // Check if file reading is complete
        if (!respData.hasNextChunk())
        {
            std::string lastChunk = "0\r\n\r\n";
            send(clientSocket, lastChunk.c_str(), lastChunk.size(), 0);
            respData.closeFile();
            respData.status = ResponseData::Complete;

        }
    }
}

void Server::run()
{
    signal(SIGPIPE, SIG_IGN);
    struct epoll_event ev, events[MAX_EVENTS];

    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 10);
    if (nfds == -1)
    {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; n++)
    {
        int current_fd = events[n].data.fd;
        // std::cout << "events: " << events[n].data.fd << std::endl;
        if (current_fd == sockfd)
        {
            struct sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int new_socket = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
            if (new_socket < 0)
            {
                perror("accept");
                continue;
            }
            setNonBlocking(new_socket);
            ev.events = EPOLLIN | EPOLLOUT;
            ev.data.fd = new_socket;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1)
            {
                perror("epoll_ctl: new_socket");
                close(new_socket);
                exit(1);
            }
            client_socket[new_socket] = new_socket;
            requests[new_socket] = HTTPRequest();
            clientResponses[new_socket] = ResponseData();
            // std::cout << "helllllo\n";
        }
        else
        {
            if (events[n].events & EPOLLOUT && requests[current_fd].respense)
            {
                // std::cout << "std: " << std::endl;
                if (clientResponses.find(current_fd) != clientResponses.end())
                {
                    // std::cout << "alah n3arfo lmochkil\n";
                    if (requests[current_fd].method == "GET")
                        handleRequestGET(current_fd, requests[current_fd], serverConfig);
                    else if (requests[current_fd].method == "DELETE")
                        handleRequestDELETE(current_fd, requests[current_fd], serverConfig);
                    ResponseData& respData = clientResponses[current_fd];
                    // if (respData.status == ResponseData::InProgress)   
                    sendResponseChunk(current_fd, respData);

                    if (respData.status == ResponseData::Complete)
                    {
                        requests.erase(current_fd);
                        close(current_fd);
                        client_socket[n] = 0;
                        clientResponses.erase(current_fd);
                    }
                }
            }
            else if (events[n].events & EPOLLIN && !requests[current_fd].respense)
            {
                try
                {
                    if (!requests[current_fd].headersComplete)
                        requests[current_fd].readHeaders(current_fd, requests[current_fd]);
                    if (requests[current_fd].headersComplete)
                    {
                        if (requests[current_fd].method == "GET" || requests[current_fd].method == "DELETE")
                            requests[current_fd].respense = true;
                        else if (requests[current_fd].method == "POST")
                            handleRequestPOST(current_fd, requests[current_fd]);
                        else
                        {
                            sendErrorResponse(current_fd, 501, "Not Implemented");
                            requests[current_fd].clear();
                        }
                        // requests[current_fd].clear();
                        // close(current_fd);
                        // client_socket[n] = 0;
                        // requests.erase(current_fd);
                    }
                }
                catch (std::runtime_error& e)
                {
                    sendErrorResponse(current_fd, 400, e.what());
                }
            }
        }
    }
}


int main(int argc, char **argv)
{
    try
    {
        configFile cFile(argc, argv);
        servers start(cFile);
        std::map<int, informations> info = start.getMap();
        int size = 0;
        std::map<int, informations>::iterator it = info.begin();
        for(;it != info.end(); ++it)
            size++;
        std::cout << "size: " << size << std::endl;

        std::map<std::string, int> ListenTosockets;

        std::vector<Server> AllServer;
        for (int i = 0; i < size; i++)
        {
            informations config = start.getServerInfo(i);
            std::string port = config.port["listen"];
            std::string host = config.host["host"];
            std::string hostPortKey = host + ":" + port;
            int socketFd = 0;
            if (ListenTosockets.find(hostPortKey) == ListenTosockets.end())
            {
                socketFd = createAndBindSocket(port, host);
                ListenTosockets[hostPortKey] = socketFd;
                Server myServer(socketFd, config, port, host);
                myServer.listenToSocket();
                myServer.initializeEpoll();
                AllServer.push_back(myServer);
            }
            else
            {
                socketFd = ListenTosockets[hostPortKey];
                Server myServer(socketFd, config, port, host);
                myServer.initializeEpoll();
                AllServer.push_back(myServer);
            }
            std::cout << "Server is running..." << std::endl;
        }
        while(true)
        {
            for(Server& server : AllServer)
                server.run();
        }
            

    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}