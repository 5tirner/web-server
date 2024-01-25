#include "mainHeader.hpp"

/*This File For Any Code About String Manupilution*/

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