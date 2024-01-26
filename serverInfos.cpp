#include "mainHeader.hpp"

informations servers::serverInfos(void)
{
    informations tmp;
    size_t       i = 0;
    while (i < this->server.size())
    {
        for (size_t j = 0; j < this->server.size(); j++)
        {
            
        }
    }
    return (tmp);
}
int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        if (isAgoodServer(this->server[i]))
            return (1);
        //this->serversInfo[i] = info;
    }
    return (0);
}