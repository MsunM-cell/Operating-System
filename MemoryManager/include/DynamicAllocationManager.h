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

    //�Ѿ������ҳ������һ���������ڴ档
    int occupiedPageNum;

    //ʹ���е�֡����Ŀ
    int usedFrameNum;

    int swapPageNum;

    //ʹ��getInstance�ɻ�ø�ʵ��
    static DynamicAllocationManager *instance;

    //��¼֡ʹ�������˫���...ѭ����...
    FrameTableItem *LRU_StackHead, *LRU_StackTail;

    // pid--���̱�
    map<int, ProcessTableItem *> processTable;

    //֡��
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