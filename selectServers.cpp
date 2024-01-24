#include "mainHeader.hpp"

servers::servers(void) {}

servers::~servers(void){}

servers::servers(const servers &other){*this = other;}

servers&servers::operator=(const servers &other)
{
    this->my_map = other.my_map;
    return (*this);
}

servers::servers(configFile &cFile)
{
    
}