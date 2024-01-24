#ifndef MAINHEADER_HPP
# define MAINHEADER_HPP

#include <exception>
#include <fstream>
# include <iostream>
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

class   locations
{
    private:
        std::string                 location;
        std::vector<std::string>    indexes;
    public:
        locations(void);
        locations(const locations &other);
        locations&operator=(const locations &other);
        ~locations(void);
};

class   servers
{
    private:
        std::map<int , locations *> my_map;
    public:
        servers(void);
        servers(configFile &cFile);
        servers(const servers &other);
        servers&operator=(const servers &other);
        ~servers(void);
};

#endif