/*
 * @Date: 2022-05-06 19:37:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 23:31:44
 * @FilePath: \test\main.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */

 #include "base.cpp"
// #include"childA.cpp"
int main()
{
    base* b =  base::getInstance();
    b->printName();
    delete b;
    return 0;

    
    //�޷�����new A����
    //��Ϊͷ�ļ�����˳��Ϊ childA.cpp -> childA.h -> base.h,�����޷�������base.cpp ????
    // childA* a = new childA;
    // a->printName();
    // delete a;
}