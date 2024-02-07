#include <cstring>
# include <iostream>
#include <string>
# include <unistd.h>

int response(std::string a, std::string b)
{
    int fd[2];
    if (pipe(fd) == -1)
        return (1);
    else
    {
        write(fd[1], a.c_str(), a.size());
        std::cout << "Message `" + a  + "` Is Here" << std::endl;
        char *c = (char *)malloc(a.size());
        read(fd[0], c, a.size());
        std::cout << "Message `" + b + "` Is Here To Response `" + a + "`" << std::endl;
    }
    return (0);
}

int main(int ac, char **av)
{
    if (ac != 3)
        std::cout << "Please Enter Your Virtual Comminucation" << std::endl;
    else
        response(av[1], av[2]);
}