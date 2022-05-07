#include"childA.h"

childA::childA(/* args */)
{
    a = 100;
}

childA::~childA()
{
    std::cout<<"free a"<<std::endl;
}

void childA::printName()
{
    std::cout << "this is a A , and a = " << a << std::endl;
}
