#include "mainHeader.hpp"
#include <string>

servers::servers(void) {}

servers::~servers(void){}

servers::servers(const servers &other){*this = other;}

servers&servers::operator=(const servers &other)
{
    this->my_map = other.my_map;
    return (*this);
}

servers::servers(configFile &cFile)
{
    std::string buffer;
    std::string needed;
    while (std::getline(cFile.getFile(), buffer))
    {
        if (buffer[0] == '#' || buffer[0] == '\n')
            continue;
        bool checker = 0;
        if (buffer[0] == ' ' || buffer[0] == '\t')
        {
            for(unsigned long i = 0; i < buffer.size(); i++)
            {
                if (buffer[i] == ' ' || buffer[i] == '\t')
                {
                    if (i + 1 < buffer.size() && buffer[i + 1] == '#')
                    {
                        checker = 1; break;
                    }
                }
            }
        }
        if (!checker)
        {
            std::cout << buffer << std::endl;
            needed += buffer;
        }
    }
}