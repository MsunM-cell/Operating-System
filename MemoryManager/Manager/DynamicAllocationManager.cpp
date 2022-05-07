/*
 * @Date: 2022-04-29 14:55:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 21:39:22
 * @FilePath: \Operating-System\MemoryManager\Manager\DynamicAllocationManager.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
#include "../include/DynamicAllocationManager.h"

DynamicAllocationManager *DynamicAllocationManager::instance = nullptr;

DynamicAllocationManager *DynamicAllocationManager::getInstance()
{
    if (instance)
    {
        return instance;
    }
    instance = new DynamicAllocationManager();
    return instance;
}

DynamicAllocationManager::DynamicAllocationManager()
{
    LRU_StackHead = nullptr;
    LRU_StackTail = nullptr;
    PAGE_NUM = mem_config.FRAME_NUM + mem_config.SWAP_MEMORY_SIZE / mem_config.PAGE_SIZE;
    char *pointer = memory;
    for (int i = 0; i < mem_config.FRAME_NUM; i++)
    {
        FrameTableItem *fti = new FrameTableItem((long long)pointer, i);
        frameTable.push_back(fti);
        pointer += mem_config.PAGE_SIZE;
    }
    usedFrameNum = 0;
    swapPageNum = 0;
    initPageTable();
}

DynamicAllocationManager::~DynamicAllocationManager()
{
    delete memory;
}

/**
 * @brief �õ�һ�����̶�Ӧ��ҳ��
 * @param {int} pid
 * @return {�ý��̶�Ӧ��ҳ��}
 */
ProcessTableItem *DynamicAllocationManager::getProcess(int pid)
{
    auto iter = processTable.find(pid);
    if (iter == processTable.end())
    {
        return nullptr;
    }
    return iter->second;
}

/**
 * @brief ����pid�Լ���Ҫ����ĳ��ȣ�Ϊ�ý��̷����ڴ�
 * @param {unsigned int} pid
 * @param {long long} length
 * @return {1Ϊ�ɹ�������ʧ��}
 */
int DynamicAllocationManager::createProcess(PCB &p)
{
    cout << "very ok!" << endl;
    // // FIXME:����û���ļ���ַŶ�����ԾͰ���Ҫ�ĳ��ȵ����ļ����Ȱ�
    // ProcessTableItem *psti = new ProcessTableItem(p.id, p.size, p.size);

    // int allocPageNum = p.size % mem_config.PAGE_SIZE == 0 ? p.size / mem_config.PAGE_SIZE : p.size / mem_config.PAGE_SIZE + 1;
    // if (PAGE_NUM - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
    // {
    //     stringstream ss;
    //     ss << "There is no enough Memory(" << (PAGE_NUM - occupiedPageNum) * mem_config.PAGE_SIZE << ") for " << p.size;
    //     Log::logE(TAG, ss.str());
    //     return -1;
    // }

    // // TODO:bitmap��֪����û�д��ڵı�Ҫ��

    // int findPage = 0;

    // stringstream ss;
    // ss << "process " << p.id << " ask for " << allocPageNum << " pages";
    // Log::logV(TAG, ss.str());

    // for (int i = 0; i < PAGE_NUM && findPage < allocPageNum; i++)
    // {
    //     if (!bitMap[i])
    //     {
    //         bitMap[i] = true;
    //         tableItem *item = new tableItem;
    //         item->pageNo = i;
    //         item->isChange = 0;
    //         item->isInMemory = false;
    //         item->frame = nullptr;
    //         item->swapAddress = -1;
    //         pageTable->push_back(item);
    //         findPage++;
    //     }
    // }
    // occupiedPageNum += allocPageNum;
    // return 1;
}