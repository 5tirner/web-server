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
    it = tmp.errorPage.begin();
    std::cout << "ErrorPage " << it->first << " - " << "|"+it->second+"|" << std::endl;
}

int     complecatedValues(std::string &value)
{
    (void)value;
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
        if (complecatedValues(save))
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