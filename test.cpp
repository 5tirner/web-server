#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <vector>
int main()
{
    // char s[] = "www.1337.ma";
    // struct addrinfo *hints, *res;
    // hints = new struct addrinfo();
    // int status = getaddrinfo(s, "80", hints, &res);
    // if (status == 0) {
    //     std::cout << "Zero: Good Infos" << std::endl;

    //     char ip[100];
    //     inet_ntop(res->ai_addr->sa_family, &res->ai_addr->sa_data[2], ip, sizeof(ip));
    //     std::cout << "The Address Ip: " << ip << std::endl;
    //     delete hints;
    // }
    // else if (status == EAI_ADDRFAMILY)
    //     std::cout << "EAI_ADDFAM: network host does not have any network" << std::endl;
    // else if (status == EAI_AGAIN)
    //     std::cout << "EAI_AGAIN: server returned a temporary" << std::endl;
    // else if (status == EAI_BADFLAGS)
    //     std::cout << "EAI_BADF: hints.ai_flags  contains  invalid  flags" << std::endl;
    // else if (status == EAI_FAIL)
    //     std::cout << "EAI_FAIL: name server returned a permanent failure" << std::endl;
    // else if (status == EAI_FAMILY)
    //     std::cout << "EAI_FAMILY: The requested address family is not supported" << std::endl;
    // else if (status == EAI_MEMORY)
    //     std::cout << "EAI_MEM: Out of memory" << std::endl;
    // else if (status == EAI_NODATA)
    //     std::cout << "EAI_NODATA: not have any network addresses defined" << std::endl;
    // else if (status == EAI_NONAME)
    //     std::cout << "EAI_NONAME: node  or service is not known" << std::endl;
    // else if (status == EAI_SOCKTYPE)
    //     std::cout << "EAI_SOCKTYPE: requested socket type is not supported" << std::endl;
    // else if (status == EAI_SERVICE)
    //     std::cout << "EAI_SERVICE: The  requested service is not available" << std::endl;
    // else if (status == EAI_SYSTEM)
    //     std::cout << "EAI_SYSTEM: Other system error" << std::endl;
    std::vector<int> save = {3,2,4};
    std::vector<int>::iterator it = std::find(save.begin(), save.end(), 2);
    std::cout << it - save.begin() << std::endl;
}