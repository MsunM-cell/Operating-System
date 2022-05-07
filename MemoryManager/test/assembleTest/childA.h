/*
 * @Date: 2022-05-06 19:24:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 23:24:36
 * @FilePath: \test\childA.h
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */





#pragma once
#include "base.h"
#include"iostream"
class childA : public base
{
private:
    /* data */
public:
    childA(/* args */);
    ~childA();
    static childA* getInstance(){
        return new childA();
    }
    void printName();
    int a;
};

