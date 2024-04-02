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
    //     std::cerr << "Zero: Good Infos" << std::endl;

    //     char ip[100];
    //     inet_ntop(res->ai_addr->sa_family, &res->ai_addr->sa_data[2], ip, sizeof(ip));
    //     std::cerr << "The Address Ip: " << ip << std::endl;
    //     delete hints;
    // }
    // else if (status == EAI_ADDRFAMILY)
    //     std::cerr << "EAI_ADDFAM: network host does not have any network" << std::endl;
    // else if (status == EAI_AGAIN)
    //     std::cerr << "EAI_AGAIN: server returned a temporary" << std::endl;
    // else if (status == EAI_BADFLAGS)
    //     std::cerr << "EAI_BADF: hints.ai_flags  contains  invalid  flags" << std::endl;
    // else if (status == EAI_FAIL)
    //     std::cerr << "EAI_FAIL: name server returned a permanent failure" << std::endl;
    // else if (status == EAI_FAMILY)
    //     std::cerr << "EAI_FAMILY: The requested address family is not supported" << std::endl;
    // else if (status == EAI_MEMORY)
    //     std::cerr << "EAI_MEM: Out of memory" << std::endl;
    // else if (status == EAI_NODATA)
    //     std::cerr << "EAI_NODATA: not have any network addresses defined" << std::endl;
    // else if (status == EAI_NONAME)
    //     std::cerr << "EAI_NONAME: node  or service is not known" << std::endl;
    // else if (status == EAI_SOCKTYPE)
    //     std::cerr << "EAI_SOCKTYPE: requested socket type is not supported" << std::endl;
    // else if (status == EAI_SERVICE)
    //     std::cerr << "EAI_SERVICE: The  requested service is not available" << std::endl;
    // else if (status == EAI_SYSTEM)
    //     std::cerr << "EAI_SYSTEM: Other system error" << std::endl;
    std::vector<int> save = {3,2,4};
    std::vector<int>::iterator it = std::find(save.begin(), save.end(), 2);
    std::cerr << it - save.begin() << std::endl;
}