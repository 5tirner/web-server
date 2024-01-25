#include "mainHeader.hpp"

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        this->server[i].find("listen");
        this->server[i].find("server_name");
    }
    return (0);
}