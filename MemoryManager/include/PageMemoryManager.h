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
