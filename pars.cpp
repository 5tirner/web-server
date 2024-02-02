#include "mainHeader.hpp"
#include <cstddef>

/*This File For Any Code About String Manupilution*/

int    getContent(std::string &server)
{
    std::string save;
    bool        start = 0;
    size_t      end = server.size() - 1;
    while (end > 0 && server[end] != '}')
    {
        if (server[end] != ' ' && server[end] != '\t' && server[end] != '\n')
            return (1);
        end--;
    }
    for (size_t i = 0; i < end - 1; i++)
    {
        if (start == 1)
            save.push_back(server[i]);
        else
        {
            if (server[i] == '{')
            {
                start = 1;
                while (i < server.size() && (server[i] == ' ' || server[i] == '\t'
                    || server[i] == '\n'))
                    i++;
                i++;
            }
        }
    }
    server = save;
    return (0);
}

int isAgoodServer(std::string &server)
{
    int openCollad = 0;
    int closeCollad = 0;
    for (size_t i = 0; i < server.size(); i++)
    {
        if (server[i] == '{' || server[i] == '}')
        {
            if (server[i] == '{') openCollad++;
            else if (server[i] == '}') closeCollad++;
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
    }
    if (closeCollad != openCollad)
        return (1);
    if (getContent(server))
        return (1);
    return (0);
}

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
                if (s[i] == '#')
                    return (ret + "\n");
                ret.push_back(s[i]);
                i++;
            }
        }
    }
    return (ret);
}