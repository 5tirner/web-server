#include "../include/mainHeader.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

int main(int ac, char **av, char **env)
{
    if (ac == 2)
    {
        char *interpreter = strdup("/bin/php-cgi");
        char **args = (char **)malloc(sizeof(char *) * 3);
        args[0] = strdup("offset");
        args[1] = strdup(av[1]);
        args[2] = NULL;
        int processDup = fork();
        if (!processDup)
        {
            execve(interpreter, args, env);
            std::cerr << "Error: Execve Failed." << std::endl;
        }
        else if (processDup == -1)
            std::cerr << "Error: Fork Failed To Create A New Process." << std::endl;
        else
            while (waitpid(processDup, NULL, WUNTRACED) == -1);
        free(interpreter);
        for (int i = 0; args[i]; i++)
            free(args[i]);
        free(args);
        std::cout << "Done;" << std::endl;
        std::string save;
        while (std::getline(std::cin, save));
    }
    else
        std::cerr << "Only Two Argements Please." << std::endl; 
}