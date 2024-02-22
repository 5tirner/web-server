#include "../include/mainHeader.hpp"
#include <sstream>
#include <string>
#include <unistd.h>

void    connection::fillRequest(std::string buffer, std::map<int, int>::iterator &it)
{
    // std::cout << buffer << std::endl;
    // if (this->Requests.at(it->first).dataFile.is_open())
    // {

    // }
    // else
    // {
    //     std::stringstream   stream; stream << it->first;
    //     std::string Fname; stream >> Fname;
    //     this->Requests.at(it->first).dataFile.open(Fname.c_str(), std::ios::out | std::ios::app);
    //     if (this->Requests.at(it->first).dataFile)
    //         std::cout << "File For Holding Data For Client " << it->first << " Opened With Name " + Fname + "." << std::endl;
    //     else
    //         std::cerr << "Error: Fialed To Open File For Client " << it->first << "." << std::endl;
    // }
}