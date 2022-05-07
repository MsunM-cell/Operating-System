/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-29 15:23:16
 * @FilePath: \Operating-System\MemoryManager\include\DynamicAllocationManager.h
 */
#ifndef DYNAMIC_ALLOCATION
#define DYNAMIC_ALLOCATION

#include "ProcessTableItem.h"
#include "../Manager/mem.h"
#include "FrameTableItem.h"
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <string.h>
#include "../Entity/Log.cpp "

using namespace std;

class DynamicAllocationManager : public MemoryManager
{
private:
    std::string TAG = "DynamicAllocationManager";

    //已经申请的页数，不一定分配了内存。
    int occupiedPageNum;

    //使用中的帧的数目
    int usedFrameNum;

    int swapPageNum;

    //使用getInstance可获得该实例
    static DynamicAllocationManager *instance;

    //记录帧使用情况，双向的...循环的...
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--进程表
    map<int, ProcessTableItem *> processTable;

    //帧表
    vector<FrameTableItem *> frameTable;

    ProcessTableItem *getProcess(int pid);

public:
    long PAGE_NUM;
    static DynamicAllocationManager *getInstance();
    DynamicAllocationManager();
    ~DynamicAllocationManager();
    int createProcess(PCB &p);
    int freeProcess(PCB &p);
    char accessMemory(int pid, long long address);
    void initPageTable();
    int getOccupiedPageNum();
    void useFrame(FrameTableItem *frameTableItem);
    int getUsedFrameNum() { return usedFrameNum; };
    int getSwapPageNum() { return swapPageNum; };

    bool pageFault(unsigned int pid, tableItem *ti);
    int writeMemory(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif