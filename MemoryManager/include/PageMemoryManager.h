/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-18 17:13:05
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#pragma once

#include "../Manager/mem.h"
#include "../Entity/FrameTableItem.cpp"
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

    int load_ins(int pid, string file_address);

public:
    long PAGE_NUM;

    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(PCB &p);
    int freeProcess(PCB &p);
    char accessMemory(int pid, int address_index);
    void initPageTable();
    void useFrame(FrameTableItem *frameTableItem);
    unsigned long getLogicalMemorySize() { return mem_config.MEM_SIZE + mem_config.SWAP_MEMORY_SIZE; };
    bool pageFault(unsigned int pid, tableItem *ti);
    int writeMemory(int logicalAddress_index, char src, unsigned int pid);
    // void stuff(unsigned int pid);

    int getUsedFrameNum() { return usedFrameNum; };
    int getOccupiedPageNum();
    int getSwapPageNum() { return swapPageNum; };
};
