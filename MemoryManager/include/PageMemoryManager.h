/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 19:42:14
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#include "../Interface/MemoryManager-1.cpp"
#include"FrameTableItem.h"
#include "config.h"
#include <vector>
#include <map>
#include<iostream>
#include "../Interface/FileManager.cpp"


FileManager fileManager;
using namespace std;

struct tableItem
{
    int frameNo;
    unsigned long pageNo;
    bool isInMemory;
    int accessTime;
    bool isChange;
    unsigned long  swapAddress;
    bool isLock;
    tableItem *next , *pre;
};

class PageMemoryManager : public MemoryManager
{
private:
    bool *bitMap;
    char *memory;
    int occupiedPageNum;

    tableItem* LRU_StackHead , *LRU_StackTail;

    map<int, vector<tableItem*> *> tableMap;
    vector<FrameTableItem*> frameTable;
    vector<tableItem*> *getProcessPageTable(int pid);
    void usePage(tableItem* ti);

public:
    PageMemoryManager();
    ~PageMemoryManager();
    int memoryAlloc(unsigned int pid, unsigned long length);
    bool memoryFree(int pid, int address, int length);
    bool freeAll(int pid);
    char accessMemory(int pid, int address);
    void initPageTable();
    int getOccupiedPageNum();
    bool pageFault(unsigned int pid, tableItem* ti);
    bool write(unsigned long logicalAddress, const void *src, unsigned long size, unsigned int pid);
    
};