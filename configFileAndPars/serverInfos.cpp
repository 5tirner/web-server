#include "../include/mainHeader.hpp"

void    showInfo2(informations &tmp)
{
    std::cout << "For Locations" << std::endl;
    for (size_t i = 0; i < tmp.locationsInfo.size(); i++)
    {
        std::cout << "Location - " + tmp.locationsInfo[i].directory << std::endl;
        std::cout << "root - " + tmp.locationsInfo[i].root << std::endl;
        std::cout << "index - " + tmp.locationsInfo[i].index << std::endl;
        std::cout << "methodes - " + tmp.locationsInfo[i].allowed_methodes << std::endl;
        std::cout << "autoidx - " + tmp.locationsInfo[i].autoindex << std::endl;
        std::cout << "return - " + tmp.locationsInfo[i].Return << std::endl;
        std::cout << "upload - " + tmp.locationsInfo[i].upload << std::endl;
        std::cout << "cgi - " + tmp.locationsInfo[i].cgi << std::endl;
    }
}
void    showInfo(informations &tmp)
{
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "For Server" << std::endl;
    std::map<std::string, std::string>::iterator it = tmp.port.begin();
    std::cout << "Port " << it->first << " - " << it->second << std::endl;
    it = tmp.host.begin();
    std::cout << "Host " << it->first << " - " << it->second << std::endl;
    it = tmp.serverName.begin();
    std::cout << "ServerName " << it->first << " - " << it->second << std::endl;
    it = tmp.limitClientBody.begin();
    std::cout << "LimitClient " << it->first << " - " << it->second << std::endl;
    it = tmp.errorPage.begin();
    std::cout << "ErrorPage " << it->first << " - " << it->second << std::endl;
}

int checkLocations(informations &tmp)
{
    for (size_t i = 0; i < tmp.locations.size(); i++)
    {
        std::stringstream input(tmp.locations[i]);
        std::string       buffer;
        location          save;
        while (std::getline(input, buffer))
        {
            std::string key;
            size_t j = 0;
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
                save.directory = &buffer[j];
            else if (key == "root")
                save.root = &buffer[j];
            else if (key == "index")
                save.index = &buffer[j];
            else if (key == "allowed_methodes")
                save.allowed_methodes = &buffer[j];
            else if (key == "autoindex")
                save.autoindex = &buffer[j];
            else if (key == "return")
                save.Return = &buffer[j];
            else if (key == "upload")
                save.upload = &buffer[j];
            else if (key == "cgi")
                save.cgi = &buffer[j];
            // else
            //     return (1);
        }
        tmp.locationsInfo.push_back(save);
    }
    showInfo2(tmp);
    return (0);
}
int checkInformations(informations &tmp)
{
    //std::cout << "Others" << std::endl;
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
            return (1);
        if (key == "listen")
            tmp.port[key] = &tmp.others[i][j];
        else if (key == "host")
            tmp.host[key] = &tmp.others[i][j];
        else if (key == "server_name")
            tmp.serverName[key] = &tmp.others[i][j];
        else if (key == "error_page")
            tmp.errorPage[key] = &tmp.others[i][j];
        else if (key == "limit_client_body")
            tmp.limitClientBody[key] = &tmp.others[i][j];
        else
            return (1);
    }
    showInfo(tmp);
    return (0);
}

int servers::serverInfos(int i)
{
    informations tmp;
    int          check = 0;
    std::string  save;
    std::stringstream input(this->server[i]);
    std::string buffer;
    while (std::getline(input, buffer))
    {
        //std::cout << "line= " + buffer << std::endl;
        if (!std::strncmp(buffer.c_str(), "location", 8))
        {
            //std::cout << "Find Location Word: " + buffer << std::endl; 
            check = 1;
            save += buffer + "\n";
        }
        else if (check == 0)
            tmp.others.push_back(buffer);
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
        return (1);
    std::cout << "Server Number " << i << " Informations" << std::endl;
    size_t j = 0;
    std::cout << "Not Location" << std::endl;
    while (j < tmp.others.size())
    {
        std::cout << "-> " << tmp.others[j] << std::endl;
        j++;
    }
    std::cout << "Location" << std::endl;
    j = 0;
    while (j < tmp.locations.size())
    {
        std::cout << "-> " << tmp.locations[j] << std::endl;
        j++;
    }
    if (checkInformations(tmp) || checkLocations(tmp))
        return (1);
    //this->serversInfo[i] = tmp;
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