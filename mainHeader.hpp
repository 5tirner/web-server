#ifndef MAINHEADER_HPP
# define MAINHEADER_HPP

#include <exception>
#include <fstream>
# include <iostream>
#include <utility>
# include <vector>
# include <map>
# include <string>

class   configFile
{
    private:
        std::string  fileName;
        std::fstream File;
    public:
        configFile(void);
        configFile(int ac, char **av);
        configFile(const configFile &other);
        configFile&operator=(const configFile &other);
        ~configFile(void);
        std::fstream &getFile();
        class   BadRunException : public std::exception
        {
            const char* what() const throw()
            {
                return ("executable should run as follows: ./webserv [configFile]");
            }
        };
        class   BadFileException : public std::exception
        {
            const char* what() const throw()
            {
                return ("Bad Config File Name");
            }
        };
};

typedef struct info
{
    std::pair<std::string, int> port;
    std::pair<std::string, std::string> serverName;
}   informations;

class   servers
{
    private:
        std::vector<informations> server;
    public:
        servers(void);
        servers(configFile &cFile);
        servers(const servers &other);
        servers&operator=(const servers &other);
        ~servers(void);
};

std::string removeWhiteSpaces(std::string &buffer);

#endif