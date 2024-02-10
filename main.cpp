#include "./include/mainHeader.hpp"

int main(int ac, char **av)
{
    std::map<int, informations> general;
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile);
        general = start.getMap();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    std::map<int, informations>::iterator it = general.begin();
    while (it != general.end())
    {
        std::cout << "Server Number " << it->first + 1 << ":" << std::endl;
        std::cout << "About Server:" << std::endl;
        showInfo(it->second);
        std::cout << "About Locations:" << std::endl;
        showInfo2(it->second);
        std::cout << "////////////////////////////////////////////////" << std::endl;
        it++;
    }
}