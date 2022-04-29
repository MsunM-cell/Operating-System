/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-29 09:55:34
 * @FilePath: \Operating-System\MemoryManager\include\PageMemoryManager.h
 */
#ifndef PAGE_MEMORY_MANAGER
#define PAGE_MEMORY_MANAGER

#include "../Manager/mem.h"
#include "FrameTableItem.h"
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

    //�Ѿ������ҳ������һ���������ڴ档
    int occupiedPageNum;

    //ʹ���е�֡����Ŀ
    int usedFrameNum;

    int swapPageNum;

    //ʹ��getInstance�ɻ�ø�ʵ��
    static PageMemoryManager *instance;

    //��¼֡ʹ�������˫���...ѭ����...
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--ҳ��ӳ��
    map<int, vector<tableItem *> *> tableMap;

    //֡��
    vector<FrameTableItem *> frameTable;

    vector<tableItem *> *getProcessPageTable(int pid);

public:
    long PAGE_NUM;

    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(PCB &p);
    int freeProcess(PCB &p);
    char accessMemory(int pid, long long address);
    void initPageTable();
    int getOccupiedPageNum();
    void useFrame(FrameTableItem *frameTableItem);
    int getUsedFrameNum() { return usedFrameNum; };
    int getSwapPageNum() { return swapPageNum; };
    unsigned long getLogicalMemorySize() { return mem_config.MEM_SIZE + mem_config.SWAP_MEMORY_SIZE; };
    bool pageFault(unsigned int pid, tableItem *ti);
    int writeMemory(long long logicalAddress, const void *src, long long size, unsigned int pid);
    void stuff(unsigned int pid);
};

#endif