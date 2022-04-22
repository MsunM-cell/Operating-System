/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-22 18:55:41
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#ifndef PAGE_MEMORY_MANAGER
#define PAGE_MEMORY_MANAGER

#include "../Interface/mem.h"
#include "../Manager/FrameTableItem.cpp"
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
    
    // //物理内存
    // char *memory;
    

    //已经申请的页数，不一定分配了内存。
    int occupiedPageNum;

    //使用中的帧的数目
    int usedFrameNum;

    int swapPageNum;
 
    //使用getInstance可获得该实例
    static PageMemoryManager *instance;

    //LRU链表头及尾
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--页表映射
    map<int, vector<PageTableItem *> *> tableMap;

    //帧表
    vector<FrameTableItem *> frameTable;

    vector<PageTableItem *> *getProcessPageTable(int pid);

    void useFrame(FrameTableItem *frameTableItem);
    void initPageTable();
    bool pageFault(unsigned int pid, PageTableItem *ti);
public:
    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(PCB& p);
    int freeProcess(PCB& p);
    char accessMemory(int pid, long long address);
    int getOccupiedPageNum();
    int getUsedFrameNum() { return usedFrameNum; };
    int getSwapPageNum() { return swapPageNum; };
    unsigned long getLogicalMemorySize() { return mem_config.MEM_SIZE + mem_config.SWAP_MEMORY_SIZE; };
    int writeMemory(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif