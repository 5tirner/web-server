#include "./include/mainHeader.hpp"
#include <exception>

int main(int ac, char **av)
{
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile);   
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
}