#include <iostream>
#include <sstream>

int main()
{
    int a = 10;
    int b = 20;
    std::stringstream stream;
    stream << a << "-" << b;
    std::string str;
    stream >> str;
    std::cout << str << std::endl; 
}