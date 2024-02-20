#include "./include/Server.hpp"
#include "./include/mainHeader.hpp"
#include "./include/HTTPRequest.hpp"


int main(int argc, char **argv)
{
    try
    {
        configFile cFile(argc, argv);
        servers    start(cFile.getFile());
        std::map<int, informations> info = start.getMap();
        int size = 0;
        std::map<int, informations>::iterator it = info.begin();
        for(;it != info.end(); ++it)
            size++;
        std::cout << "size: " << size << std::endl;

        std::map<std::string, int> ListenTosockets;

        std::vector<Server> AllServer;
        for (int i = 0; i < size; i++)
        {
            informations config = start.getServerInfo(i);
            std::string port = config.port["listen"];
            std::string host = config.host["host"];
            std::string hostPortKey = host + ":" + port;
            int socketFd = 0;
            if (ListenTosockets.find(hostPortKey) == ListenTosockets.end())
            {
                socketFd = createAndBindSocket(port, host);
                ListenTosockets[hostPortKey] = socketFd;
                Server myServer(socketFd, config, port, host);
                myServer.listenToSocket();
                myServer.initializeEpoll();
                AllServer.push_back(myServer);
            }
            else
            {
                socketFd = ListenTosockets[hostPortKey];
                Server myServer(socketFd, config, port, host);
                myServer.initializeEpoll();
                AllServer.push_back(myServer);
            }
            std::cout << "Server is running..." << std::endl;
        }
        while(true)
        {
            std::vector<Server>::iterator it;
            for (it = AllServer.begin(); it != AllServer.end(); ++it) {
                it->run();
            }

            // for(Server& server : AllServer)
            //     server.run();
        }
            

    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}