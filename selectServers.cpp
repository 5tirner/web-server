#include "mainHeader.hpp"
#include <cstddef>
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
    std::string all;
    while (std::getline(cFile.getFile(), buffer))
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
            }
        }
        if (!checker)
            needed += removeWhiteSpaces(buffer);
    }
    std::cout << "-> All Things: " << std::endl
    << all << "-> What We Need: " << std::endl << needed;
}