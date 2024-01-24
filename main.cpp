#include "mainHeader.hpp"
#include <exception>

int main(int ac, char **av)
{
    try
    {
        configFile cFile(ac, av);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}