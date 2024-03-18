#include "../include/mainHeader.hpp"

std::map<int, informations> &servers::getMap(void)
{
    return (this->serversInfo);
}

servers::servers(void) {}

servers::~servers(void){}

servers::servers(const servers &other){*this = other;}

servers&servers::operator=(const servers &other)
{
    for (size_t i = 0 ; i < other.server.size(); i++)
        this->server[i] = other.server[i];
    return (*this);
}

int servers::isolateServers(std::string &s)
{
    size_t check = s.find("server");
    if (check != 0)
    {
        std::cout << "Problem In The Top Of The File" << std::endl;
        return (1);
    }
    std::string save;
    for (size_t i = 0; i < s.size(); i++)
    {
        int checker = isServer(s, i);
        if (!checker && i != 0)
        {
            this->server.push_back(save);
            save.clear();
        }
        else if (checker == 2) return (1);
        save.push_back(s[i]);
    }
    if (save.size())
        this->server.push_back(save);
    return (0);
}

servers::servers(std::fstream &cFile)
{
    std::string buffer;
    std::string needed;
    std::string all;
    while (std::getline(cFile, buffer))
    {
        buffer.push_back('\n');
        all += buffer;
        if (buffer[0] == '#' || buffer[0] == '\n')
            continue;
        bool checker = 0;
        if (buffer[0] == ' ' || buffer[0] == '\t')
        {
            for(size_t i = 0; i < buffer.size(); i++)
            {
                if (buffer[i] == ' ' || buffer[i] == '\t')
                {
                    if (i + 1 < buffer.size() && buffer[i + 1] == '#')
                    {
                        checker = 1; break;
                    }
                }
                else
                    break;
            }
        }
        if (!checker)
            needed += removeWhiteSpaces(buffer);
    }
    // std::cout << "All Things\n" << all << std::endl;
    // std::cout << "What We Need\n" << needed << std::endl;
    // std::cout << "-------------------------------------------" << std::endl;
    // std::cout << "I will Isolate Servers" << std::endl;
    if (this->isolateServers(needed))
    {
        cFile.close();
        throw BadConetent();
    }
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Server Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
    //std::cout << "--------------------------------------------" << std::endl;
    // std::cout << "I will Fill Infos" << std::endl;
    if (this->fillInfos())
    {
        cFile.close();
        throw BadConetent();
    }
    // std::map<int, informations>::iterator it = this->serversInfo.begin();
    // while (it != this->serversInfo.end())
    // {
    //     std::cout << "Server Number " << it->first + 1 << ":" << std::endl;
    //     std::cout << "About Server:" << std::endl;
    //     showInfo(it->second);
    //     std::cout << "About Locations:" << std::endl;
    //     showInfo2(it->second);
    //     std::cout << "////////////////////////////////////////////////" << std::endl;
    //     it++;
    // }
    // std::cout << "--------------------------------------------" << std::endl;
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Content Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
    cFile.close();
}
