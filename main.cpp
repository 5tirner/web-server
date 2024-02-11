#include "./include/mainHeader.hpp"

void showmeConfigthings(std::map<int, informations> &general)
{
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
int main(int ac, char **av)
{
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile);
        //showmeConfigthings(start.getMap());
        connection doMuluplexing(start.getMap());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
}