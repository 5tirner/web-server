#include "../include/mainHeader.hpp"

configFile::configFile(void) {}

configFile::~configFile(void) {}

configFile::configFile(const configFile &other) {*this = other;}

configFile &configFile::operator=(const configFile &other)
{
    this->fileName = other.fileName;
    return (*this);
}

configFile::configFile(int ac, char **av)
{
    if (ac == 1)
        this->fileName = "./conf/server.conf";
    else if (ac == 2)
        this->fileName = av[1];
    else
        throw BadRunException();
    // ?std::cerr << "Config File Name: " + this->fileName << std::endl; 
    this->File.open(this->fileName.c_str(), std::ios_base::in);
    if (!this->File)
        throw BadFileException();
}

std::fstream &configFile::getFile()
{
    return (File);
}