/*
 * @Date: 2022-05-06 19:20:16
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 21:34:38
 * @FilePath: \test\base.h
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */

#pragma once
#include <iostream>
int mode = 1;

class base
{
private:
public:
    virtual void printName()
    {
        std::cout << "....ok i'm base" << std::endl;
    }
    base();
    virtual ~base();
    static base *instance;
    static base *getInstance();
};
