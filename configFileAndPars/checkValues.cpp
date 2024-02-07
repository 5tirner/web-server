# include "../include/mainHeader.hpp"

int normalCheck(std::string &value)
{
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
    //std::cout << value + " Become " + save << std::endl;
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
            //std::cout << "---->" << check << std::endl;
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
    }
    else if (save == "off")
        values = "off";
    else
        return (1);
    return (0);
}
int multiValues(std::string &key, std::string &values)
{
    if (key == "index" || key == "server_name")
    {
        if (justMakeItRight(values))
            return (1);
    }
    else if (key == "allowed_methodes")
    {
        if (justMakeItRight(values) || methodesSyntax(values))
            return (1);
    }
    else if (key == "cgi")
    {
        if (justMakeItRight(values) || cgiAndUploadSyntax(values))
            return (1);
    }
    else if (key == "upload")
    {
        if (justMakeItRight(values) || cgiAndUploadSyntax(values))
            return (1);
    }
    else if (key == "return")
    {
        if (justMakeItRight(values))
            return (1);
    }
    return (0);
}