#include <fstream>
#include <iostream>
#include <cstdio>
#include <unistd.h>

int main()
{
    if (!freopen("testing", "w+", stdout))
        std::cerr << "Error: Freopen Failed." << std::endl;
    else
    {
        std::cout << "Inside The File" << std::endl;
        std::cout << "Trunc Succefully" << std::endl;
        fclose(stdout);
        sleep(10);
        std::cout << "Done." << std::endl;
    }
}