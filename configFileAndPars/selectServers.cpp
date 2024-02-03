#include "../include/mainHeader.hpp"

servers::servers(void) {}

servers::~servers(void){}

servers::servers(const servers &other){*this = other;}

servers&servers::operator=(const servers &other)
{
    for (size_t i = 0 ; i < other.server.size(); i++)
        this->server[i] = other.server[i];
    return (*this);
}

servers::servers(configFile &cFile)
{
    std::string buffer;
    std::string needed;
    std::string all;
    while (std::getline(cFile.getFile(), buffer))
    {
        buffer.push_back('\n');
        all += buffer;
        if (buffer[0] == '#' || buffer[0] == '\n')
        {
            continue;
        }
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
    std::cout << "-------------------------------------------" << std::endl;
    if (this->isolateServers(needed))
        throw BadConetent();
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Server Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
    std::cout << "--------------------------------------------" << std::endl;
    if (this->fillInfos())
        throw BadConetent();
    // std::cout << "--------------------------------------------" << std::endl;
    // for (size_t i = 0; i < this->server.size(); i++)
    // {
    //     std::cout << "Content Number " << i << ":\n"
    //     << this->server[i] << std::endl;
    // }
}

int servers::isolateServers(std::string &s)
{
    size_t check = s.find("server");
    if (check != 0)
        return (1);
    std::string save;
    for (size_t i = 0; i < s.size(); i++)
    {
        int checker = isServer(s, i);
        if (!checker && i != 0)
        {
            this->server.push_back(save);
            save.clear();
        }
        else if (checker == 2)
            return (1);
        save.push_back(s[i]);
    }
    if (save.size())
        this->server.push_back(save);
    return (0);
}