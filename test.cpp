
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
    std::map<std::string, std::string>  autoindex;
    std::map<std::string, std::string>  allowed_methodes;
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
int         valueCheck(informations &tmp);
int         isAgoodValue(std::string &value, char c);

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
                    std::cout << "Wrong Thing after ; int this:\n" + server << std::endl;
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
    // std::cout << "All Things\n" << all << std::endl;
    // std::cout << "What We Need\n" << needed << std::endl;
    // std::cout << "-------------------------------------------" << std::endl;
    std::cout << "I will Isolate Servers" << std::endl;
    if (this->isolateServers(needed))
        throw BadConetent();
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Server Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
    //std::cout << "--------------------------------------------" << std::endl;
    std::cout << "I will Fill Infos" << std::endl;
    if (this->fillInfos())
        throw BadConetent();
    std::map<int, informations>::iterator it = this->serversInfo.begin();

    while (it != this->serversInfo.end())
    {
        if (valueCheck(it->second))
            throw BadConetent();
        std::cout << "Server Number " << it->first + 1 << ":" << std::endl;
        std::cout << "About Server:" << std::endl;
        showInfo(it->second);
        std::cout << "About Locations:" << std::endl;
        showInfo2(it->second);
        std::cout << "////////////////////////////////////////////////" << std::endl;
        it++;
    }
    // std::cout << "--------------------------------------------" << std::endl;
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Content Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
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
    it = tmp.errorPage.begin();
    std::cout << "ErrorPage " << it->first << " - " << "|"+it->second+"|" << std::endl;
}

int     complecatedValues(std::string &value, char c)
{
    //std::cout << "rush -> " + value << std::endl;
    if (c == 'M')
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

    }
    else if (c == 'C')
    {

    }
    else if (c == 'U')
    {

    }
    else if (c == 'R')
    {

    }
    return (0);
}

int     isAgoodValue(std::string &value, char c)
{
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
    if (save.size() == 0 || save == ";")
        return (1);
    if (c == 'N')
    {
        if (strchr(save.c_str(), ' ') || strchr(save.c_str(), '\t' ))
            return (1);
    }
    else
    {
        if (complecatedValues(save, c))
            return (1);
    }
    value = save;
    return (0);
}

int     valueCheck(informations &tmp)
{
    std::map<std::string, std::string>::iterator it = tmp.port.begin();
    if (isAgoodValue(it->second, 'N') || atoi(it->second.c_str()) <= 0)
    { std::cout << "Not A valid Port " + it->second << std::endl; return (1);}
    for (size_t i = 0; i < it->second.size(); i++)
    {
        if (!isdigit(it->second[i]))
        { std::cout << "Not A valid Port " + it->second << std::endl; return (1);}
    }
    it = tmp.host.begin();
    if (isAgoodValue(it->second, 'N'))
    { std::cout << "Not A valid Host " + it->second << std::endl; return (1);}
    it = tmp.serverName.begin();
    if (isAgoodValue(it->second, 'N'))
    { std::cout << "Not A valid ServerName " + it->second << std::endl; return (1); }
    it = tmp.limitClientBody.begin();
    if (isAgoodValue(it->second, 'N'))
    { std::cout << "Not A valid LimitClientBody " + it->second << std::endl; return (1); }
    for (size_t i = 0; i < it->second.size(); i++)
    {
        if (!isdigit(it->second[i]))
        {
            std::cout << "Not A valid Port " + it->second << std::endl;
            return (1);
        }
    }
    it = tmp.errorPage.begin();
    if (isAgoodValue(it->second, 'N'))
    { std::cout << "Not A valid ErrorPage " + it->second << std::endl; return (1); }
    for (size_t i = 0; i < tmp.locationsInfo.size(); i++)
    {
        it = tmp.locationsInfo[i].directory.begin();
        if (isAgoodValue(it->second, 'N'))
        { std::cout << "Not A Valid Location " + it->second << std::endl; return (1);}
        it = tmp.locationsInfo[i].root.begin();
        if (isAgoodValue(it->second, 'N'))
        { std::cout << "Not A Valid Root " + it->second << std::endl; return (1);}
        it = tmp.locationsInfo[i].autoindex.begin();
        if (isAgoodValue(it->second, 'N'))
        { std::cout << "Not A Valid AutoIndex " + it->second << std::endl; return (1);}
        if (it->second != "on" && it->second != "off")
        { std::cout << "Not A Valid AutoIndex " + it->second << std::endl; return (1);}
        it = tmp.locationsInfo[i].allowed_methodes.begin();
        if (isAgoodValue(it->second, 'M'))
        { std::cout << "Not A Valid Methodes " + it->second << std::endl; return (1);}
        if (isAgoodValue(it->second, 'I'))
        { std::cout << "Not A Valid Indexes " + it->second << std::endl; return (1); }
    }
    return (0);
}

void    initialLocation(location &save)
{
    save.directory["location"] = "No_Location";
    save.root["root"] = "No_root";
    save.index["index"] = "index.html";
    save.allowed_methodes["allowed_methodes"] = "GET";
    save.autoindex["autoindex"] = "off";
    save.Return["return"] = "0";
    save.upload["upload"] = "off";
    save.cgi["cgi"] = "off";
}

void    etatInitial(informations &tmp)
{
    tmp.port["listen"] = "No_Port";
    tmp.host["host"] = "No_Host";
    tmp.serverName["server_name"] = "defualt";
    tmp.errorPage["error_page"] = "No_Error_Page";
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
            if (key == "location") save.directory[key] = &buffer[j];
            else if (key != "location" && key != "{"
                && key != "}" && !strchr(&buffer[j], ';'))
            {
                std::cout << "Can't Find ; here " + buffer << std::endl;
                return (1);
            }
            else if (key == "root") save.root[key] = &buffer[j];
            else if (key == "index") save.index[key] = &buffer[j];
            else if (key == "allowed_methodes") save.allowed_methodes[key] = &buffer[j];
            else if (key == "autoindex") save.autoindex[key] = &buffer[j];
            else if (key == "return") save.Return[key] = &buffer[j];
            else if (key == "upload") save.upload[key] = &buffer[j];
            else if (key == "cgi") save.cgi[key] = &buffer[j];
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
        std::string key;
        size_t j = 0;
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
        {
            std::cout << "Can't Find ; Here " + tmp.others[i] << std::endl; 
            return (1);
        }
        if (key == "listen") tmp.port[key] = &tmp.others[i][j];
        else if (key == "host") tmp.host[key] = &tmp.others[i][j];
        else if (key == "server_name") tmp.serverName[key] = &tmp.others[i][j];
        else if (key == "error_page") tmp.errorPage[key] = &tmp.others[i][j];
        else if (key == "limit_client_body") tmp.limitClientBody[key] = &tmp.others[i][j];
        else
        {
            std::cout << "Bad KeyWord In Server Data" + key << std::endl; 
            return (1);
        }
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
    //std::cout << "Server Number " << i << " Informations" << std::endl;
    //size_t j = 0;
    //std::cout << "Not Location" << std::endl;
    // while (j < tmp.others.size())
    // {
    //     std::cout << "-> " << tmp.others[j] << std::endl;
    //     j++;
    // }
    // std::cout << "Location" << std::endl;
    // j = 0;
    // while (j < tmp.locations.size())
    // {
    //     std::cout << "-> " << tmp.locations[j] << std::endl;
    //     j++;
    // }
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

int main(int ac, char **av)
{
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile); 
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
}