#include "../include/mainHeader.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
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
            this->types[".ruby"] = "/bin/ruby";
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
            {
                this->executer = "/bin/php-cgi";
            }
        }
        std::string &getExecuter(void)
        {
            return (this->executer);
        }
};

class   CGI
{
    private:
        int             fd[2];
        std::fstream    cgiFile;
    public:
        CGI(){}
        CGI(std::string method, char *av, char **env)
        {
            std::cout << "- FileName: " << av << std::endl;
            if (method == "GET")
            {
                Executer    obj(av);
                char    *args[3];
                args[0] = av;
                args[1] = av;
                args[2] = NULL;
                if (pipe(this->fd) == -1)
                {
                    std::cerr << "Error: Failed To Create A PIPE." << std::endl;
                    throw 1;
                }
                // if (dup2(fd[1], 1) == -1)
                //     exit(1);
                int processDup = fork();
                if (!processDup)
                {
                    execve(obj.getExecuter().c_str(), args, env);
                    std::cerr << "Error: Execve Failed." << std::endl;
                    close(this->fd[1]), close(this->fd[0]);
                    throw 1;
                }
                else if (processDup == -1)
                {
                    std::cerr << "Error: Fork Failed To Create A New Process." << std::endl;
                    close(this->fd[1]), close(this->fd[0]);
                    throw 1;
                }
                else
                    while (waitpid(processDup, NULL, WUNTRACED) == -1);
                std::string save;
                while (std::getline(std::cin, save));
            }
        }
};

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        CGI cgi("GET", av[1], env);
    }
    else
        std::cerr << "Only Two Argements Please." << std::endl; 
}