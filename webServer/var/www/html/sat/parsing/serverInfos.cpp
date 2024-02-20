#include "../includes/mainHeader.hpp"

int isAgoodServer(std::string &server)
{
    int openCollad = 0;
    int closeCollad = 0;
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i] == '{' || server[i] == '}')
        {
            i++;
            while (i < server.size() && server[i] != '\n')
            {
                if (server[i] != ' ' && server[i] != '\t')
                {
                    //std::cout << '`' << server[i] << '`' << std::endl;
                    return (1);
                }
                i++;
            }
        }
        if (server[i] == ';')
        {
            i++;
            while (i < server.size() && server[i] != '\n')
            {
                if (server[i] != ' ' && server[i] != '\t' && server[i] != ';')
                {
                    //std::cout << '`' << server[i] << '`' << std::endl;
                    return (1);
                }
                i++;
            }
        }
        if (server[i] == '}') closeCollad++;
        else if (server[i] == '{') openCollad++;
    }
    if (closeCollad != openCollad)
        return (1);
    return (0);
}

int servers::fillInfos(void)
{
    for (size_t i = 0; i < this->server.size(); i++)
    {
        if (isAgoodServer(this->server[i]))
            return (1);
        // informations info;
        // this->server[i].find("listen");
        // this->server[i].find("server_name");
    }
    return (0);
}