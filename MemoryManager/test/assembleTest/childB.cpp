#include"childB.h"


childB::childB(/* args */)
{
    for (int i = 0; i < 10; i++)
    {
        b[i] = i;
    }
}

childB::~childB()
{
    std::cout<<"free b" <<std::endl;
}

void childB::printName()
{
    std::cout << "this is a B , and b = " << std::endl;
    for (int i = 0; i < 10; i++)
    {
        std::cout << b[i] << " ";
    }
    std::cout << std::endl;
}