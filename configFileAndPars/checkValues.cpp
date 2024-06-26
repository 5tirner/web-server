# include "../include/mainHeader.hpp"
#include <map>

int normalCheck(std::string &value)
{
    if (value.size() == 0)
        return (1);
    std::string save;
    size_t i = 0, j = value.size() - 1;
    while ((i < value.size()) && (value[i] == ' ' || value[i] == '\t'))
        i++;
    while ((j) && (value[j] == ' ' || value[j] == '\t'
            || value[j] == '\n' || value[j] == ';' || value[j] == '{'))
        j--;
    while (i <= j)
    {
        save.push_back(value[i]);
        i++;
    }
    //std::cerr << value + " Become " + save << std::endl;
    if (save.size() == 0 || save == ";") return (1);
    if (strchr(save.c_str(), ' ') || strchr(save.c_str(), '\t' ))
        return (1);
    value = save;
    return (0);
}

int     methodesSyntax(std::string &value)
{
    std::string check;
    int Post = 0, Get = 0, Delete = 0; 
    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == ' ' || value[i] == '\t')
        {
            //std::cerr << "---->" << check << std::endl;
            if (check == "POST") Post++;
            else if (check == "DELETE") Delete++;
            else if (check == "GET") Get++;
            else return (1);
            if (Post > 1 || Get > 1 || Delete > 1)
                return (1);
            check.clear();
        }
        else
            check.push_back(value[i]);
    }
    if (check.size())
    {
        if (check == "POST") Post++;
        else if (check == "DELETE") Delete++;
        else if (check == "GET") Get++;
        else return (1);
        if (Post > 1 || Get > 1 || Delete > 1)
            return (1);
    }
    return (0);
}

int    justMakeItRight(std::string &values)
{
    size_t i = 0, j = values.size() - 1;
    std::string save;
    while ((i < values.size()) && (values[i] == ' ' || values[i] == '\t'))
        i++;
    while ((j > 0) && (values[j] == ' ' || values[j] == '\t'
            || values[j] == ';' || values[j] == '\n'))
            j--;
    while (i <= j)
    {
        save.push_back(values[i]);
        i++;
    }
    if (save == ";" || save.size() == 0)
        return (1);
    values = save;
    return (0);
}

int cgiAndUploadSyntax(std::string &values)
{
    std::string save;
    size_t i = 0;
    int check = 0;
    for (; i < values.size(); i++)
    {
        if ((i != 0) && (values[i] != ' ' && values[i] != '\t'))
        {
            if (values[i - 1] == ' ' || values[i - 1] == '\t')
                check++;
        }
    }
    if (check > 1)
        return (1);
    i = 0;
    for(; (i < values.size()) && (values[i] != ' ' && values[i] != '\t'); i++)
        save.push_back(values[i]);
    if (save == "on")
    {
        while ((i < values.size()) && (values[i] == ' ' || values[i] == '\t'))
            i++;
        values = &values[i];
        if (values.size() == 0)
            return (1);
    }
    else if (save == "off")
        values = "";
    else
        return (1);
    return (0);
}

int isInteger(std::string &value, char c)
{
    if ((c == 'P') && (atoi(value.c_str()) < 1024 || atoi(value.c_str()) > 65535))
        return (1);
    else if (c == 'N' && atoi(value.c_str()) <= 0)
        return (1);
    for (size_t i = 0; i < value.size(); i++)
    {
        if (!isdigit(value[i]))
            return (1);
        if (c == 'P' && i > 4)
            return (1);
    }
    return (0);
}

int redirection(int *status, std::string &val)
{
    if (justMakeItRight(val))
        return (1);
    std::string save;
    size_t i = 0;
    for (; i < val.size(); i++)
    {
        if (val[i] == ' ' || val[i] == '\t')
            break;
        else if (!std::isdigit(val[i]))
            return (1);
        save.push_back(val[i]);
    }
    if (save.size() != 3)
        return (1);
    for (; i < val.size(); i++)
    {
        if (val[i] != ' ' && val[i] != '\t')
            break;
    }
    val = &val[i];
    if (!val.size())
        return (1);
    *status = std::atoi(save.c_str());
    if (*status < 300 || *status > 399)
    {
        std::cerr << "Cause Of Status Code, ";
        return (1);
    }
    return (0);
}

int isValidIp4(std::string &value)
{
    std::string save;
    int point = 0;
    for (size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == '.')
        {
            if (save.size() == 0)
                return (1);
            point++;
            if (point > 3)
                return (1);
            if (save.size() > 3 || atoi(save.c_str()) > 255)
                return (1);
            if (save.size() > 1 && save[0] == '0')
                return (1);
            save.clear();
        }
        else if (!isdigit(value[i]))
            return (1);
        else
            save.push_back(value[i]);
    }
    if (save.size() == 0)
        return (1);
    else
    {
        if (save.size() > 3 || atoi(save.c_str()) > 255)
                return (1);
        if (save.size() > 1 && save[0] == '0')
                return (1);
    }
    if (point != 3)
        return (1);
    return (0);
}

int multiValues(std::string &key, std::string &values)
{
    if (key == "index")
    {
        if (justMakeItRight(values))
            return (1);
    }
    else if (key == "allowed_methodes")
    {
        if (justMakeItRight(values) || methodesSyntax(values))
            return (1);
    }
    else if (key == "upload")
    {
        if (justMakeItRight(values) || cgiAndUploadSyntax(values))
            return (1);
    }
    return (0);
}

int errorPages(std::string &all, int *status)
{
    if (justMakeItRight(all))
        return (1);
    //std::cerr << "ErrorPage Appears As: " + all << std::endl;
    std::string save;
    size_t      i = 0;
    while (i < all.size() && all[i] != ' ' && all[i] != '\t')
    {
        if (all[i] == ' ' || all[i] == '\t')
            break;
        else if (!std::isdigit(all[i]))
            return (1);
        save.push_back(all[i]);
        i++;
    }
    if (!save.size())
        return (1);
    if (save.size() != 3)
        return (1);
    for (; i < all.size(); i++)
    {
        if (all[i] != ' ' && all[i] != '\t')
            break;
    }
    all = &all[i];
    if (!all.size())
        return (1);
    *status = std::atoi(save.c_str());
    if (*status > 599)
    {
        std::cerr << "Cause Of Status Code, ";
        return (1);
    }
    return (0);
}
