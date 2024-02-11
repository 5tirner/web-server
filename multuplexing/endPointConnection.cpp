#include "../include/mainHeader.hpp"

int errorsGenerator(std::string error, int fd)
{
    std::cerr << error << std::endl;
    if (fd != 0)
        close(fd);
    return (1);
}

connection::connection(void){}

connection::connection(const connection &other){*this = other;}

connection &connection::operator=(const connection &other)
{
    this->configData = other.configData;
    return (*this);
}

connection::~connection(void){}

connection::connection(std::map<int, informations> &configData)
{
    (void)configData;
}

int main()
{
    int servereFd = socket(AF_INET, SOCK_STREAM, 0);
    if (servereFd == -1)
        return (errorsGenerator("Can't Create Endpoint", 0));
    std::cout << "New Endpoint Connection " << servereFd << " Ready To Use." << std::endl;
    struct  sockaddr_in ipv4;
    ipv4.sin_port = htons(8080);
    ipv4.sin_family = AF_INET;
    socklen_t   len = sizeof(ipv4);
    int fill = 1;
    int setSock = setsockopt(servereFd, SOL_SOCKET, SO_REUSEADDR, &fill, sizeof(fill));
    if (setSock)
        return (errorsGenerator("setSockopt Failed", servereFd));
    int getSocketName = bind(servereFd, (struct sockaddr *)&ipv4, len);
    if (getSocketName == -1)
        return (errorsGenerator("Can't Assigning A Name For The Socket", servereFd));
    std::cout << "Socket Number " << servereFd << " Get Name." << std::endl;
    int turnToPassiveSocket = listen(servereFd, 1);
    if (turnToPassiveSocket == -1)
        return (errorsGenerator("Listen Failed To Make The Socket Passive", servereFd));
    std::cout << "Socket Number " << servereFd << " Is Passive Now." << std::endl
    << "Waiting For An Endpoint From Client Side..." << std::endl;
    int clientFd = accept(servereFd, (struct sockaddr *)&ipv4, &len);
    if (clientFd == -1)
        return (errorsGenerator("Can't Create Endpoint For The Client", servereFd));
    std::cout << "Client Number " << clientFd << " Is Here." << std::endl;
    char buffer[1024];
    while (1)
    {
        int checker = read(clientFd, buffer, 1024);
        if (checker == -1)
        {
            close(clientFd);
            return (errorsGenerator("Can't Read From Clien Endpoint", servereFd));
        }
        else if (checker == 0)
        {
            std::cout << "Nothing Left To Read It From Client Endpoint, Go Check Client Side, GOOD BYE :D." << std::endl;
            break;
        }
        else
        {
            buffer[checker] = '\0';
            write(1, buffer, checker);
        }
        if (write(clientFd, "Hi Client From Server Side", strlen("Hi Client From Server Side")) == -1)
            std::cout << "Can't Write Into Client Endpoint" << std::endl;
    }
    close(clientFd);
    close(servereFd);
}