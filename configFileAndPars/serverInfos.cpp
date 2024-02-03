#include "../include/mainHeader.hpp"
#include <cstring>

void    showInfo(informations &tmp)
{
    std::cout << "----------------------------------------" << std::endl;
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

int checkInformations(informations &tmp)
{
    //std::cout << "Others" << std::endl;
    for (size_t i = 0; i < tmp.others.size(); i++)
    {
        std::string key;
        size_t j = 0;
        for (; j < tmp.others[i].size(); j++)
        {
            if (tmp.others[i][j] == ' ')
            {
                if (j < tmp.others[i].size())
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
        if (!std::strncmp(buffer.c_str(), "location", 8))
        {
            //std::cout << "Find Location Word: " + buffer << std::endl; 
            check = 1;
            save += buffer;
        }
        else if (check == 0)
            tmp.others.push_back(buffer);
        else if (check == 1)
        {
            save += buffer;
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
    if (checkInformations(tmp))
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