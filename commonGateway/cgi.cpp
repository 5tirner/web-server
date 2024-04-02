#include "../include/mainHeader.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <ostream>
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
// std::string cgiFile(std::string &fileName, char **env, std::string &executor, bool *flag)
// {
//     std::string outputFileName = fileName + "_cgi.html";
//     pid_t pid = fork();

//     if (pid == -1) {
//         // Fork failed
//         perror("fork");
//         throw std::runtime_error("Failed to create a new process.");
//     } else if (pid == 0) {
//         // Child process
//         int outFile = open(outputFileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//         if (outFile == -1) {
//             perror("open");
//             exit(EXIT_FAILURE);
//         }
//         if (dup2(outFile, STDOUT_FILENO) == -1) { // Redirect stdout to outFile
//             perror("dup2");
//             exit(EXIT_FAILURE);
//         }
//         close(outFile); // outFile no longer needed after dup2

//         // Prepare arguments for execve
//         char *args[3];
//         args[0] = const_cast<char *>(executor.c_str());
//         args[1] = const_cast<char *>(fileName.c_str());
//         args[2] = NULL;

//         execve(executor.c_str(), args, env);
//         // If execve returns, it's an error
//         perror("execve");
//         exit(EXIT_FAILURE);
//     } else {
//         // Parent process
//         int status;
//         while (waitpid(pid, &status, 0) == -1) {
//             if (errno != EINTR) {
//                 perror("waitpid");
//                 throw std::runtime_error("Error waiting for the CGI process to finish.");
//             }
//         }

//         if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//             std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
//             *flag = true; // Indicate failure if necessary
//         }

//         // Check if output file is empty
//         std::ifstream outputFileStream(outputFileName.c_str());
//         outputFileStream.seekg(0, std::ios::end);
//         if (outputFileStream.tellg() == 0) {
//             *flag = true; // Indicate empty output file
//         }
//         execve(executer.c_str(), args, NULL);
//         exit(10000);
//     }

//     return outputFileName;
// }


void cgiFile(cgiInfo& cgiInfo)
{
    std::cerr << "1- FileName: " << cgiInfo.script << std::endl;
    char *args[3] = {
        (char *)cgiInfo.binary.c_str(),
        (char *)cgiInfo.script.c_str(),
        NULL
        };
    std::stringstream str;
    std::srand(std::time(NULL));
    str << ".cgi_file" << std::rand() << std::endl; 
    str >> cgiInfo.output;
    int processDup1 = fork();
    if (!processDup1)
    {
        std::cout << "\033[35mfdsfffsdf" << cgiInfo.output.c_str() << "\033[35m"  << "\033[0m" << std::endl;
        char *Env[] = {
            (char*)strdup(("REQUEST_METHOD=" + cgiInfo.method).c_str()),
            (char*)"REDIRECT_STATUS=200",
            (char*)strdup(("SCRIPT_FILENAME=" + cgiInfo.script).c_str()),
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


       // change the path to the path info chdri();


        execve(cgiInfo.binary.c_str(), args, Env);
        exit(150);
    }
    cgiInfo.pid = processDup1;
    cgiInfo.startTime = std::clock();
}
 