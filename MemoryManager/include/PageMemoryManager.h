/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-14 19:49:06
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

    //�߼�ҳ��ʹ��λͼ
    bool *bitMap;
    
    //�����ڴ�
    char *memory;

    //�Ѿ������ҳ������һ���������ڴ档
    int occupiedPageNum;

    //ʹ���е�֡����Ŀ
    int usedFrameNum;

    int swapPageNum;
 
    //ʹ��getInstance�ɻ�ø�ʵ��
    static PageMemoryManager *instance;

    //LRU����ͷ��β
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--ҳ��ӳ��
    map<int, vector<PageTableItem *> *> tableMap;

    //֡��
    vector<FrameTableItem *> frameTable;

    vector<PageTableItem *> *getProcessPageTable(int pid);

    void useFrame(FrameTableItem *frameTableItem);
    void initPageTable();
    bool pageFault(unsigned int pid, PageTableItem *ti);
public:
    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(unsigned int pid, long long length);
    bool memoryFree(unsigned int pid, long long address, long long length);
    bool freeAll(unsigned int pid);
    char accessMemory(unsigned int pid, long long address);
    int getOccupiedPageNum();
    int getUsedFrameNum() { return usedFrameNum; };
    int getSwapPageNum() { return swapPageNum; };
    unsigned long getLogicalMemorySize() { return LOGICAL_MEMORY_SIZE; };
    bool write(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif