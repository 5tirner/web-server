#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>

std::string GetExtension(std::string &filename)
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
    // std::cerr << "- Extention: " << &filename[i] << std::endl;
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

void cgiFile(cgiInfo& cgiInfo)
{
    // std::cerr << "1- FileName: " << cgiInfo.script << std::endl;
    char *args[3] = {
        (char *)cgiInfo.binary.c_str(),
        (char *)cgiInfo.script.c_str(),
        NULL
        };
    std::stringstream str;
    std::srand(std::time(NULL));
    str << ".cgi_file" << std::rand() << std::endl; 
    std::cout << ".SCRIPTE NAME:" << cgiInfo.script << std::endl; 
    std::cout  << ".PATH INFO:" << cgiInfo.pathInfo << std::endl; 
    str << ".cgi_file" << std::rand() << std::endl; 
    str >> cgiInfo.output;
    std::cerr << "Script Form CGIINFO is: " +  cgiInfo.script << std::endl;
    int processDup1 = fork();
    if (!processDup1)
    {
        char *Env[] = {
            (char*)strdup(("REQUEST_METHOD=" + cgiInfo.method).c_str()),
            (char*)"REDIRECT_STATUS=200",
            (char*)strdup(("SCRIPT_FILENAME=" + cgiInfo.script).c_str()), //filename only
            (char*)strdup(("QUERY_STRING=" + cgiInfo.queries).c_str()),
            (char*)strdup(("PATH_INFO=" + cgiInfo.pathInfo).c_str()),
            (char*)strdup(("HTTP_COOKIE=" + cgiInfo.cookies).c_str()),
            NULL,
            NULL,
            NULL
        };
        if (!freopen(cgiInfo.output.c_str(), "w", stdout))
            exit(150);
        if (cgiInfo.method == "POST")
        {
            Env[6] = (char*)strdup(("CONTENT_LENGTH=" + cgiInfo.contentLength).c_str());
            Env[7] = (char*)strdup(("CONTENT_TYPE=" + cgiInfo.contentType).c_str());
            if (!freopen(cgiInfo.input.c_str(), "r", stdin))
                exit(150);
        }
        execve(cgiInfo.binary.c_str(), args, Env);
        exit(150);
    }
    cgiInfo.pid = processDup1;
    cgiInfo.startTime = std::clock();
}
 