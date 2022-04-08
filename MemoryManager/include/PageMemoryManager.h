/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:27:30
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#include "../Interface/MemoryManager-1.cpp"
#include "FrameTableItem.h"
#include "../include/config.h"
#include <vector>
#include <map>
#include <iostream>
#include <string.h>

#include "../Interface/FileManager.cpp"


FileManager fileManager;
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
    int memoryAlloc(unsigned int pid, unsigned long length);
    bool memoryFree(unsigned int pid, unsigned long address, unsigned long length);
    bool freeAll(unsigned int pid);
    char accessMemory(unsigned int pid, unsigned long address);
    void initPageTable();
    int getOccupiedPageNum();
    bool pageFault(unsigned int pid, tableItem *ti);
    bool write(unsigned long logicalAddress, const void *src, unsigned long size, unsigned int pid);
    void stuff(unsigned int pid);
};