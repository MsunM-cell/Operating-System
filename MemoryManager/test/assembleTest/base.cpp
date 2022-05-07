#include "childA.cpp"
#include "childB.cpp"
#include "base.h"

base *base::instance = nullptr;
base *base::getInstance()
{
    if (instance)
    {
        return instance;
    }
    else
    {
        if (mode == 0)
        {
            instance = (base *)new childA();
            std::cout<<"new a childA"<<std::endl;
        }
        else if (mode == 1)
        {
            instance = (base *)new childB();
        }
        else
        {
            instance = new base();
        }

        return instance;
    }
}

base::base()
{
}

base::~base() {
    std::cout<<"free base"<<std::endl;
}

