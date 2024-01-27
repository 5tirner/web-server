#include "mainHeader.hpp"
#include <cstddef>
#include <cstring>

informations servers::serverInfos(void)
{
    informations tmp;
    size_t       i = 0;
    int          check = 0;
    std::string  save;
    while (i < this->server.size())
    {
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
        i++;
    }
    std::cout << "Not Locations" << std::endl;
    i = 0;
    while (i < tmp.others.size())
    {
        std::cout << "-> " << tmp.others[i] << std::endl;
        i++;
    }
    std::cout << "Location" << std::endl;
    i = 0;
    while (i < tmp.locations.size())
    {
        std::cout << "-> " << tmp.locations[i] << std::endl;
        i++;
    }
    return (tmp);
}
int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        if (isAgoodServer(this->server[i]))
            return (1);
        serverInfos();
        //this->serversInfo[i] = info;
    }
    return (0);
}