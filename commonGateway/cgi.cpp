#include "../include/mainHeader.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

std::string GetExtentions(std::string &filename)
{
    std::map<std::string, std::string> types;
    types[".pl"]  = "/bin/perl";
    types[".pm"]  = "/bin/perl";
    types[".py"]  = "/bin/python3";
    types[".js"]  = "/bin/js";
    types[".rb"]  = "/bin/ruby";
    types[".php"] = "/bin/php";
    size_t i = filename.size() - 1;
    for (; i > 0; i--)
    {
        if (filename[i] == '.')
            break;
    }
    std::cerr << "- Extention: " << &filename[i] << std::endl;
    std::string executer;
    try
    {
        executer = types.at(&filename[i]);
        std::cerr << "- Matched With " + executer << std::endl;
    }
    catch (...)
    { executer = "NormalFile"; }
    return (executer);
}

std::string cgiFile(std::string &FileName, char **env, std::string &executer, bool *FLAG)
{
    std::cerr << "1- FileName: " << FileName << std::endl;
    char *args[3];
    args[0] = (char *)FileName.c_str(), args[1] = (char *)FileName.c_str(), args[2] = NULL;
    std::string save = FileName;
    save += "_cgi.html";
    int processDup1 = fork();
    if (!processDup1)
    {
        if (!freopen(save.c_str(), "w+", stdout))
            throw "Error: freopen Failed Connect The File With stdout.";
        int processDup2 = fork();
        if (!processDup2)
        {
            execve(executer.c_str(), args, env);
            throw "Error: Execve Failed.";
        }
        else if (processDup2 == -1)
            throw "Error: Fork2 Failed To Create A New Process.";
        else
        {
            while (waitpid(processDup2, NULL, WUNTRACED) == -1);
            std::fstream F;
            F.open(save.c_str(), std::ios::in);
            if (!F) throw "Error: Failed To Open The File That Refered To stdout.";
            F.seekg(0, std::ios::end);
            if (F.tellg() == 0)
                *FLAG = true;
            F.close();
        }
    }
    else if (processDup1 == -1)
        throw "Error: Fork1 Failed To Create A New Process.";
    else
        while (waitpid(processDup1, NULL, WUNTRACED) == -1);
    // std::fstream F;
    // F.open(save.c_str(), std::ios::in);
    // if (!F)
    //     throw "Error: Failed To Open The File That Refered To stdout.";
    // F.seekg(0, std::ios::end);
    // std::cerr << F.tellg() << std::endl;
    // F.close();
    return (save);
}