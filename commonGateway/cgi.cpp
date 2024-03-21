#include "../include/mainHeader.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

typedef struct Executer
{
    std::map<std::string, std::string> types;
    Executer()
    {
        this->types[".c"]  = "cc";
        this->types["cpp"] = "c++";
        this->types["c++"] = "c++";
        this->types[".pl"] = "perl";
        this->types[".py"] = "python3";
        this->types[""]    = "php-fcgi";
    }
} exec;

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        std::fstream outPutDupping;
        outPutDupping.open("SaveTheOutPut", std::ios::in | std::ios::out | std::ios::trunc);
        if (!outPutDupping)
        { std::cerr << "Error: Failed To Open SaveTheOutPut File." << std::endl; return (1); }
        char *args[3];
        std::string interpreter = strdup("/bin/php-cgi");
        args[0] = av[1];
        args[1] = av[1];
        args[2] = NULL;
        int processDup = fork();
        if (!processDup)
        {
            execve(interpreter.c_str(), args, env);
            outPutDupping.close();
            std::cerr << "Error: Execve Failed." << std::endl;
            return (1);
        }
        else if (processDup == -1)
            std::cerr << "Error: Fork Failed To Create A New Process." << std::endl;
        else
            while (waitpid(processDup, NULL, WUNTRACED) == -1);
        std::cout << "Done;" << std::endl;
        std::string save;
        while (std::getline(std::cin, save));
    }
    else
        std::cerr << "Only Two Argements Please." << std::endl; 
}