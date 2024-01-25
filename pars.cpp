#include "mainHeader.hpp"
#include <cstring>
#include <string.h>
#include <string>
#include <vector>

/*This File For Any Code About String Manupilution*/

int isServer(std::string &s, size_t i)
{
    std::string check;
    int j = 0;
    for (; i < s.size(); i++)
    {
        if (j == 8)
            break;
        check.push_back(s[i]);
        j++;
    }
    if (check == "server {")
        return (0);
    return (1);
}

std::string removeWhiteSpaces(std::string &s)
{
    std::string ret;
    size_t i = 0;
    if (s[i] == ' ' || s[i] == '\t')
    {
        while ((i < s.size()) && (s[i] == ' ' || s[i] == '\t')) i++;
        if (i < s.size() && s[i] == '\n') i++;
    }
    while (i < s.size())
    {
        if (s[i] == ' ' || s[i] == '\t')
        {
            ret.push_back(s[i]);
            while ((i < s.size()) && (s[i] == ' ' || s[i] == '\t'))
                i++;
        }
        if (i < s.size())
        {
            while (i < s.size() && s[i] != ' ' && s[i] != '\t')
            {
                ret.push_back(s[i]);
                i++;
            }
        }
    }
    return (ret);
}

int servers::isolateServers(std::string &s)
{
    size_t check = s.find("server {");
    if (check != 0)
        return (1);
    std::string save;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!isServer(s, i) && i != 0)
        {
            this->server.push_back(save);
            save.clear();
        }
        save.push_back(s[i]);
    }
    if (save.size())
        this->server.push_back(save);
    return (0);
}