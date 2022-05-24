/*
 * @Date: 2022-05-24 00:05:55
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-24 00:11:54
 * @FilePath: \Operating-System\MemoryManager\test\addrTest.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
#include <iostream>
using namespace std;
int main()
{
    // char *cs = new char[10];
    char * cs = new char[10];
    for (int i = 0; i < 10; i++)
    {
        cs[i] = 50;
    }
    for (int i = 0; i < 10; i++)
    {
        cout <<(long long ) &(cs[i]) << endl;
    }
}