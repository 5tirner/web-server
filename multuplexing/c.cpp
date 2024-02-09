#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr a;
    socklen_t number;
    if (fd == -1)
        std::cout << "Fail To Create Socket" << std::endl;
    else
    {
        bind(fd, &a, number);
    //     std::cout << fd << std::endl;
    //     if (write(fd, "Hello Man", std::strlen("Hello Man")) == -1)
    //         std::cout << "I can't write anything here" << std::endl;
    //     else
    //     {
    //         char *s = (char *)malloc(std::strlen("Hello Man"));
    //         if (read(fd, s, std::strlen("Hello Man")) == -1)
    //             std::cout << "I can't read from this file" << std::endl;
    //         else
    //             std::cout << "I read This: " << s << std::endl;
    //    }
    //    while (1);
    }
}