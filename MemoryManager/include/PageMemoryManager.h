/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-13 16:27:27
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#ifndef PAGE_MEMORY_MANAGER
#define PAGE_MEMORY_MANAGER

#include "../Interface/MemoryManager-1.cpp"
#include "../Manager/FrameTableItem.cpp"
#include "config.h"
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <string.h>

using namespace std;

class PageMemoryManager : public MemoryManager
{
private:
    std::string TAG = "PageMemoryManager";

    //逻辑页的使用位图
    bool *bitMap;
    //物理内存
    char *memory;

    //已经申请的页数，不一定分配了内存。
    int occupiedPageNum;

    //使用中的帧的数目
    int usedFrameNum;

    int swapPageNum;

    //使用getInstance可获得该实例
    static PageMemoryManager *instance;

    //记录帧使用情况，双向的...循环的...
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--页表映射
    map<int, vector<tableItem *> *> tableMap;

    //帧表
    vector<FrameTableItem *> frameTable;

    vector<tableItem *> *getProcessPageTable(int pid);

    void useFrame(FrameTableItem *frameTableItem);

public:
    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(unsigned int pid, long long length);
    bool memoryFree(unsigned int pid, long long address, long long length);
    bool freeAll(unsigned int pid);
    char accessMemory(unsigned int pid, long long address);
    void initPageTable();
    int getOccupiedPageNum();
    int getUsedFrameNum() { return usedFrameNum; };
    int getSwapPageNum() { return swapPageNum; };
    unsigned long getLogicalMemorySize() { return LOGICAL_MEMORY_SIZE; };
    bool pageFault(unsigned int pid, tableItem *ti);
    bool write(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif