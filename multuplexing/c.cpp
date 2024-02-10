#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
int main()
{
    int serversock = socket(AF_INET, SOCK_STREAM, 0);
    if (serversock == -1)
        std::cout << "Kernel Busy, Socket Cannot Be Created" << std::endl;
    else
    {
        struct sockaddr_in sockADD;
        sockADD.sin_family = AF_INET;
        sockADD.sin_port = htons(8080);
        unsigned int check = sizeof(sockADD);
        int on = 1;
        int seter = setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (seter < 0)
            perror("setsockopt(SO_REUSEADDR) failed");
        else
        {
            int getName = bind(serversock, (struct sockaddr *)&sockADD, check);
            if (getName == -1)
                std::cout << "Kernel Busy, Can't Assigning A Name To The Socket Number:" << serversock << std::endl;
            else
            {
                std::cout << "socker get a name" << std::endl;
                std::cout << "server endpoint created" << std::endl;
                int passiveSock = listen(serversock, 2147483647);
                if (passiveSock == -1)
                    std::cout << "Socket Is Angry And She Refuse to LIsten To Us" << std::endl;
                else
                {
                    std::cout << "socket become ready to listening..." << std::endl;
                    int clientsock = accept(serversock, (struct sockaddr *) &sockADD, &check);
                    if (clientsock == -1)
                        std::cout << "Client Not Accepted" << std::endl;
                    else
                    {
                        std::cout << "Client That Have The Endpoint Number " << clientsock << " Accepted" << std::endl;
                        char c[1025];
                        while (1)
                        {
                            std::cout << "YOOOOOOOOOO" << std::endl;
                            int val = read(clientsock, &c, 1024);
                            std::cout << "----> " << val << std::endl;
                            if (val == 0)
                                break;
                            else if (val == -1)
                            {
                                std::cout << "Read Failed" << std::endl;
                                return (1);
                            }
                            else
                                if (write(1, c, val) == -1)
                                    return (1);
                        }
                    }
                    if (close(clientsock) == -1)
                        std::cout << "Close Failed For Client" << std::endl;
                }
            }
        }
    }
    if (close(serversock) == -1)
        std::cout << "Close Failed For Server" << std::endl;
}