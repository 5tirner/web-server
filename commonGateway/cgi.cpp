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
