#include <unistd.h>
#include <fcntl.h>
#include <iostream>

int main()
{
    char buffer[ 1024 ];
    int fd = open( "./main.cpp", O_RDONLY , 0666);
    int rc = read( fd, buffer, 1023 );
    rc = read( fd, buffer, 1023 );
    buffer[ rc ] = 0;
    std::cerr << "rc: " << rc << std::endl;
    // std::cerr << "buffer: " << buffer << std::endl;
}