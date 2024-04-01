#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
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
    types[".rb"]  = "/bin/ruby";
    types[".php"] = "/bin/php-cgi";
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

std::string cgiFile(std::string method, std::string &FileName, std::string &executer, std::string &input)
{
    std::cerr << "1- FileName: " << FileName << std::endl;
    std::cout << "2- InputFile: " << input << std::endl;
    char *args[3];
    args[0] = (char *)FileName.c_str(), args[1] = (char *)FileName.c_str(), args[2] = NULL;
    std::string save = FileName;
    save += "_cgi.html";
    if (method == "POST")
    {
        save = "SINGALAFATsda.html";
        std::cout << "In Post " << save << std::endl;
    }
    std::cout << "File To Point ON stdout " + save << std::endl;
    int processDup1 = fork();
    if (!processDup1)
    {
        if (!freopen(save.c_str(), "w+", stdout))
            throw "Error: freopen Failed Connect The File With stdout.";
        if (method == "POST")
        {
            if (!freopen(input.c_str(), "r", stdin))
                throw "Error: freopen Failed Connect The File With stdin.";
        }
        execve(executer.c_str(), args, NULL);
        exit(10000);
    }
    else if (processDup1 == -1)
        throw "Error: Fork1 Failed To Create A New Process.";
    else
        while (waitpid(processDup1, NULL, WUNTRACED) == -1);
    return (save);
}