#include "mainHeader.hpp"
#include <cstddef>
#include <string>

void takeContent(std::string &server)
{
    size_t i = 0;
    size_t j = server.size() - 1;
    while (i < server.size() && server[i] != '{')
        i++;
    while (j > 0 && server[j] != '}')
        j--;
    server = server.substr(i + 1, j - 1);
};

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        //informations info;
        if (isAgoodServer(this->server[i]))
            return (1);
        takeContent(server[i]);
        // this->serversInfo[1] = info;
    }
    return (0);
}