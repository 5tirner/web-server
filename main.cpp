#include "./include/mainHeader.hpp"

void showmeConfigthings(std::map<int, informations> &general)
{
    std::map<int, informations>::iterator it = general.begin();
    while (it != general.end())
    {
        std::cerr << "Server Number " << it->first + 1 << ":" << std::endl;
        std::cerr << "About Server:" << std::endl;
        showInfo(it->second);
        std::cerr << "About Locations:" << std::endl;
        showInfo2(it->second);
        std::cerr << "////////////////////////////////////////////////" << std::endl;
        it++;
    }
}
int main(int ac, char **av, char **env)
{
    signal(SIGPIPE, SIG_IGN);
    (void)env;
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile.getFile());
        showmeConfigthings(start.getMap());
        connection doMuluplexing(start.getMap());
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
}