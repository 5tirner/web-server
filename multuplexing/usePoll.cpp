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
    check.events = POLLIN | POLLOUT;
    std::cout << "Waitig For Events..." << std::endl;
    while (1)
    {
        if (poll(&check, 1, 100) == 1)
        {
            
        }
    }    
}