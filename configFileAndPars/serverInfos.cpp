#include "../include/mainHeader.hpp"

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