#include "../include/mainHeader.hpp"
#include <cstddef>
#include <string>

void    showInfo2(informations &tmp)
{
    for (size_t i = 0; i < tmp.locationsInfo.size(); i++)
    {
        std::cerr << "Location Number " << i + 1 << ':' << std::endl;
        std::map<std::string, std::string>::iterator it
        = tmp.locationsInfo[i].directory.begin();
        std::cerr << "Location "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].root.begin();
        std::cerr << "Root " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].index.begin();
        std::cerr << "Index " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].allowed_methodes.begin();
        std::cerr << "AllowMethodes "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].autoindex.begin();
        std::cerr << "AutoIndex " << it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].Return.begin();
        std::cerr << "Return "<< it->first + " - |" + it->second + "| Redirect It With Status=" << tmp.locationsInfo[i].returnValue << std::endl;
        it = tmp.locationsInfo[i].upload.begin();
        std::cerr << "Upload "<< it->first + " - |" + it->second+"|" << std::endl;
        it = tmp.locationsInfo[i].cgi.begin();
        std::cerr << "Cgi " << it->first + " - |" + it->second+"|" << std::endl;
    }
}
void    showInfo(informations &tmp)
{
    std::map<std::string, std::string>::iterator it = tmp.port.begin();
    std::cerr << "Port: " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.host.begin();
    std::cerr << "Host: " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.serverName.begin();
    std::cerr << "ServerName: " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.limitClientBody.begin();
    std::cerr << "LimitClient: " << it->first << " - " << "|"+it->second+"|" << std::endl;
    it = tmp.defaultRoot.begin();
    std::cerr << "DefaultRoot " << it->first << " - " << "|"+it->second+"|" << std::endl;
    std::map<int, std::string>::iterator it1 = tmp.errorPages.begin();
    while (it1 != tmp.errorPages.end())
        std::cerr << "Error Page: " << it1->second << " With Status Code=" << it1->first << std::endl, it1++; 
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
    std::cerr << "I will Check The Location Info" << std::endl;
    for (size_t i = 0; i < tmp.locations.size(); i++)
    {
        std::stringstream input(tmp.locations[i]);
        std::string       buffer;
        location          save;
        initialLocation(save);
        save.root["root"] = tmp.defaultRoot.at("default_root");
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
                if (it->second[i] && stat(it->second.c_str(), &metadata))
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
                if (it->second[i] && stat(it->second.c_str(), &metadata))
                { std::cerr << "Invalid Upload Path " + it->second << std::endl; return (1);}
            }
            else if (key == "cgi")
            {
                save.cgi[key] = &buffer[j];
                std::map<std::string, std::string>::iterator it = save.cgi.begin(); 
                if (normalCheck(it->second))
                { std::cerr << "Invalid `Cgi` Syntax: " + it->second << std::endl; return (1); }
            }
            else if (key != "}" && key != "{")
            {
                std::cerr << "Weird KeyWord " + key << std::endl;
                return (1);
            }
        }
        tmp.locationsInfo.push_back(save);
    }
    return (0);
}

int checkInformations(informations &tmp)
{
    std::cerr << "I will Check The info" << std::endl;
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
        else if (key == "default_root")
        {
            tmp.defaultRoot[key] = &tmp.others[i][j];
            std::map<std::string, std::string>::iterator it = tmp.defaultRoot.begin();
            if (normalCheck(it->second) || !it->second[0])
            { std::cerr << "Invalid `DefualtRoot` Syntax: " + it->second << std::endl; return (1); }
            // std::cerr << "Server default root: " << it->second << std::endl;
            struct stat metadata;
            if (stat(it->second.c_str(), &metadata))
            { std::cerr << "Invalid DefaultRoot Path " + it->second << std::endl; return (1);}
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
            std::string s = &tmp.others[i][j];
            int         status;
            if (errorPages(s, &status))
            { std::cerr << "Invalid error Pages syntax: " + s << std::endl; return (1); }
            // try
            // {
            //     tmp.errorPages.at(s);
            //     std::cerr
            //     << "Error: Can't Add Multuple ErrorPage In The Same Server With Different Code Status."
            //     << std::endl;
            //     return (1);
            // }
            // catch(...)
            // {
            struct stat metadata;
            if (stat(s.c_str(), &metadata))
            { std::cerr << "Invalid ErrorPage Path " + s << std::endl; return (1);}
            tmp.errorPages[status] = s;
            //}
        }
        else
        { std::cerr << "Weird KeyWord " + key << std::endl; return (1); }
    }
    if (tmp.defaultRoot.empty())
    { std::cerr << "Error: Can't Find The DefaultRoot" << std::endl; return (1);}
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
        std::cerr << "No Location Found" << std::endl;
        return (1);
    }
    // std::cerr << "Server Number " << i << " Informations" << std::endl;
    // size_t j = 0;
    // std::cerr << "Not Location" << std::endl;
    // while (j < tmp.others.size())
    // {
    //     std::cerr << "-> " << tmp.others[j] << std::endl;
    //     j++;
    // }
    // std::cerr << "Location" << std::endl;
    // j = 0;
    // while (j < tmp.locations.size())
    // {
    //     std::cerr << "-> " << tmp.locations[j] << std::endl;
    //     j++;
    // }
    if (checkInformations(tmp) || checkLocations(tmp))
        return (1);
    this->rootLocation = false;
    for (size_t i = 0; i < tmp.locationsInfo.size(); i++)
    {
        if (tmp.locationsInfo[i].directory.at("location") == "/")
        {
            this->rootLocation = true;
            break;
        }
    }
    if (this->rootLocation == false)
    { std::cerr << "Error: Inside Server Nember: " << i + 1 << " No Root Location Founded." << std::endl; return (1);}
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