#include "mainHeader.hpp"

/*This File For Any Code About String Manupilution*/

int isServer(std::string &s, size_t i)
{
    std::string check;
    int j = 0;
    for (; i < s.size(); i++)
    {
        if (j == 7)
            break;
        check.push_back(s[i]);
        j++;
    }
    if (check == "server{") return (0);
    else if (check == "server " || check == "server\t" || check == "server\n")
    {
        for (; i < s.size(); i++)
        {
            check.push_back(s[i]);
            if (s[i] == '{')
                break;
            if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
                return (2);
        }
        if (check == "server{" || check == "server {" || check == "server\t{"
            || check == "server\n{" || check == "server \n{" || check == "server\t\n{"
            || check == "server\t \n{" || check == "server \t\n")
        {
            std::cout << "After Check ^ is a server" << std::endl;
            return (0);
        }
    }
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