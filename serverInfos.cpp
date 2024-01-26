#include "mainHeader.hpp"

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        //informations info;
        if (isAgoodServer(this->server[i]))
            return (1);
        //takeContent(server[i]);
        // this->serversInfo[i] = info;
    }
    return (0);
}