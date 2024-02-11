#include "./include/mainHeader.hpp"
#include "./include/Server.hpp"

void showmeConfigthings(std::map<int, informations> &general)
{
<<<<<<< HEAD
    std::map<int, informations> general;
=======
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
>>>>>>> b0be3a55e3b129bdf9d3286bdc61025d5bd86684
    try
    {
        configFile cFile(ac, av);
        servers    start(cFile);
<<<<<<< HEAD
        general = start.getMap();
        informations config = start.getServerInfo(0); // Implement getServerInfo in the servers class
        Server myServer(config);
        myServer.createSocket();
        myServer.bindSocket(8080, "127.0.0.1");
        myServer.listenToSocket();
        std::cout << "=========================================================================\n";
        myServer.handleConnections();
=======
        //showmeConfigthings(start.getMap());
        connection doMuluplexing(start.getMap());
>>>>>>> b0be3a55e3b129bdf9d3286bdc61025d5bd86684
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
<<<<<<< HEAD
//     std::map<int, informations>::iterator it = general.begin();
//     while (it != general.end())
//     {
//         std::cout << "Server Number " << it->first + 1 << ":" << std::endl;
//         std::cout << "About Server:" << std::endl;
//         showInfo(it->second);
//         std::cout << "About Locations:" << std::endl;
//         showInfo2(it->second);
//         std::cout << "////////////////////////////////////////////////" << std::endl;
//         it++;
//     }
=======
>>>>>>> b0be3a55e3b129bdf9d3286bdc61025d5bd86684
}