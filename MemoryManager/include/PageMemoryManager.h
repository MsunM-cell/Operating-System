/*
 * @Date: 2022-04-01 15:52:02
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-24 00:44:24
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
#include <thread>

using namespace std;

class PageMemoryManager : public MemoryManager
{
private:
    static const int LRU_ALGORITHRM = 1;
    static const int FIFO_ALGORITHRM = 2;
    int PAGE_ALGORITHRM = PageMemoryManager::FIFO_ALGORITHRM;

    std::string TAG = "PageMemoryManager";

    //�߼�ҳ��ʹ��λͼ
    bool *bitMap;

    //�����ڴ����
    long long accessTime;
    //ȱҳ����
    long long pageFaultTime;

    //�Ѿ������ҳ������һ���������ڴ档
    int occupiedPageNum;

    //ʹ���е�֡����Ŀ
    int usedFrameNum;

    int swapPageNum;

    //ʹ��getInstance�ɻ�ø�ʵ��
    static PageMemoryManager *instance;

    //��¼֡ʹ�������˫���...ѭ����...
    FrameTableItem *link_list_head, *link_list_tail;

    // pid--ҳ��ӳ��
    map<int, vector<tableItem *> *> tableMap;

    //֡��
    vector<FrameTableItem *> frameTable;

    vector<tableItem *> *getProcessPageTable(int pid);

    thread monitorThread;

    int load_ins(int pid, string file_address);

    static void monitor();
    void moveToLinkHead(FrameTableItem *fti);

public:
    long PAGE_NUM;
    static PageMemoryManager *getInstance();
    PageMemoryManager();
    ~PageMemoryManager();
    int createProcess(PCB &p);
    int freeProcess(PCB &p);
    int freeProcess(int pid);
    char accessMemory(int pid, int logicalAddress);
    void initPageTable();
    void useFrame(FrameTableItem *frameTableItem);
    bool pageFault(unsigned int pid, tableItem *ti);
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    // void stuff(unsigned int pid);

    int getUsedFrameNum() { return usedFrameNum; };
    int getOccupiedPageNum();
    int getSwapPageNum() { return swapPageNum; };
    long getPhysicalMemorySize() { return mem_config.MEM_SIZE; };
    unsigned long getLogicalMemorySize() { return mem_config.MEM_SIZE + mem_config.SWAP_MEMORY_SIZE; };
    long long getAccessTime() { return accessTime; };
    long long getPageFaultTime() { return pageFaultTime; };

    string getMode() { return "Page Memory Manager with Virtual Memory"; }
    void dms_command();
};
