#include "mainHeader.hpp"

informations servers::serverInfos(void)
{
    informations tmp;
    size_t       i = 0;
    while (i < this->server.size())
    {
        std::stringstream input(this->server[i]);
        std::string buffer;
        while (std::getline(input, buffer))
        {
            std::cout << "[ " + buffer + " ]" << std::endl;
        }
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