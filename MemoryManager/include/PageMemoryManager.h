/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 10:05:04
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#ifndef PAGEMEMORYMANAGER
#define PAGEMEMORYMANAGER

#include "../Interface/MemoryManager-1.cpp"
#include "../Manager/FrameTableItem.cpp"
#include "config.h"
#include <vector>
#include <map>
#include<sstream>
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

    //已经申请的内存量
    static PageMemoryManager *instance;
    int occupiedPageNum;
    FrameTableItem *LRU_StackHead, *LRU_StackTail;
    map<int, vector<tableItem *> *> tableMap;
    vector<FrameTableItem *> frameTable;
    vector<tableItem *> *getProcessPageTable(int pid);
    void useFrame(FrameTableItem *frameTableItem);

public:
    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int memoryAlloc(unsigned int pid, long long length);
    bool memoryFree(unsigned int pid, long long address, long long length);
    bool freeAll(unsigned int pid);
    char accessMemory(unsigned int pid, long long address);
    void initPageTable();
    int getOccupiedPageNum();
    bool pageFault(unsigned int pid, tableItem *ti);
    bool write(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif