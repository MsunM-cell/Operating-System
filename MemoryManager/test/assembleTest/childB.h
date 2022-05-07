/*
 * @Date: 2022-05-06 19:24:07
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 21:36:44
 * @FilePath: \test\childB.h
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */

#ifndef CHILDB
#define CHILDB

#include "base.h"
#include<iostream>
class childB : public base
{
private:
    /* data */
public:
    childB(/* args */);
    ~childB();
     void printName();
    int b[10];
};

#endif