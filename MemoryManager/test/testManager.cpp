/*
 * @Date: 2022-04-22 16:43:26
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-22 16:54:03
 * @FilePath: \Operating-System\MemoryManager\test\testManager.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
#include "../Interface/mem.h"

class testManager : MemoryManager
{
private:
    /* data */
public:
    testManager(/* args */);
    ~testManager();
    void test()
    {
        cout <<"block algorithm  "<< mem_config.BLOCK_ALGORITHM << endl;
        cout <<"framnum  "<< mem_config.FRAME_NUM << endl;
        cout <<"is virtual  "<< mem_config.IS_VIRTUAL << endl;
        cout <<"mem size  "<< mem_config.MEM_SIZE<<endl;
        cout <<"pagesize  "<< mem_config.PAGE_SIZE<<endl;
        cout <<"swap mem size  "<< mem_config.SWAP_MEMORY_SIZE<<endl;
    }
    int createProcess(PCB &p)
    {
        return 0;
    }
    int freeProcess(PCB &p)
    {
        return 0;
    }
    char accessMemory(int pid, long long address)
    {
        return 0;
    }
    int writeMemory(long long logicalAddress, const void *src, long long size, unsigned int pid)
    {
        return 0;
    }
};

testManager::testManager(/* args */)
{
}

testManager::~testManager()
{
}

int main()
{
    testManager m ;
    m.test();
}