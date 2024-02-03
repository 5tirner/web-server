#include "../include/mainHeader.hpp"

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
    //if (checkTheOthers(&tmp) || checTheLOcations(&tmp))
    //    return (1);
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
            throw BadConetent();
        //this->serversInfo[i] = serverInfos(i);
    }
    return (0);
}