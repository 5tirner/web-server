#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

class   Executer
{
    private:
        std::map<std::string, std::string>  types;
        std::string                         executer;
    public:
        Executer(){}
        Executer(const std::string filename)
        {
            this->types[".pl"]   = "/bin/perl";
            this->types[".pm"]   = "/bin/perl";
            this->types[".py"]   = "/bin/python3";
            this->types[".js"]   = "/bin/js";
            this->types[".rb"] = "/bin/ruby";
            size_t i = filename.size() - 1;
            for (; i > 0; i--)
            {
                if (filename[i] == '.')
                    break;
            }
            std::cout << "- Extention: " << &filename[i] << std::endl; 
            try
            {
                this->executer = this->types.at(&filename[i]);
                std::cout << "- Matched With " + this->executer << std::endl;
            }
            catch (...)
            { this->executer = "/bin/php-cgi"; }
        }
        std::string &getExecuter(void)
        { return (this->executer); }
};

class   CGI
{
    public:
        CGI(){}
        CGI(std::string method, char *av, char **env)
        {
            std::cout << "- FileName: " << av << std::endl;
            Executer    obj(av); char    *args[3];
            args[0] = av, args[1] = av, args[2] = NULL;
            int processDup1 = fork();
            if (!processDup1)
            {
                std::string save = av;
                save += ".cgi";
                if (!freopen(save.c_str(), "w+", stdout))
                    throw "Error: freopen Failed";
                int processDup2 = fork();
                if (!processDup2)
                {
                    execve(obj.getExecuter().c_str(), args, env);
                    throw "Error: Execve Failed.";
                }
                else if (processDup2 == -1)
                    throw "Error: Fork2 Failed To Create A New Process.";
                else
                    while (waitpid(processDup2, NULL, WUNTRACED) == -1);
                fclose(stdout);
            }
            else if (processDup1 == -1)
                throw "Error: Fork1 Failed To Create A New Process.";
            else
                while (waitpid(processDup1, NULL, WUNTRACED) == -1);
        }
};

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        try
        {
            CGI cgi("GET", av[1], env);
        }
        catch (const char *err)
        {
            std::cerr << err << std::endl;
        }
    }
    else
        std::cerr << "Only Two Argements Please." << std::endl; 
}