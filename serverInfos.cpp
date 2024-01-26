#include "mainHeader.hpp"

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        if (isAgoodServer(this->server[i]))
            return (1);

    }
    return (0);
}