#include <cstdlib>
#include <poll.h>
#include <sys/poll.h>
#include <unistd.h>
#include <iostream>

int main()
{
    int time;
    struct pollfd check;
    //init check;
    check.fd = 0;
    check.events = POLLOUT;
    std::cout << "Waitig For Events..." << std::endl;
    unsigned long timer = 0;
    char *buffer = (char *)malloc(1000);
    while (1)
    {
        int nfd = poll(&check, 1, 0);
        if (nfd == 1)
        {
            std::cout << "Event Happen In Fd " << nfd << std::endl;
            int p = read(0, buffer, 1000);
            if (p == -1)
            {
                free(buffer);
                return (1);
            }
            write(1, buffer, p);
            break;
        }
        else
        {
            std::cout << "HIIII" << std::endl;
            timer++;
        }
    }
    free(buffer);
    std::cout << "Late For " << timer << " Time" << std::endl; 
}