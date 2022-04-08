/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 13:39:26
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */

#include "PageMemoryManager.h"

/**
 * @description: 得到一个进程对应的页表
 * @param {int} pid
 * @return {该进程对应的页表}
 */
vector<tableItem *> *PageMemoryManager::getProcessPageTable(int pid)
{
    auto iter = tableMap.find(pid);
    vector<tableItem *> *pageTable;
    if (iter == tableMap.end())
    {
        pageTable = new vector<tableItem *>;
        tableMap.insert(pair<int, vector<tableItem *> *>(pid, pageTable));
    }
    else
    {
        pageTable = iter->second;
    }
    return pageTable;
}

/**
 * @description: 给定pid以及需要分配的长度，为该进程分配内存
 * @param {unsigned int} pid
 * @param {unsigned long} length
 * @return {1为成功，否则失败}
 */
int PageMemoryManager::memoryAlloc(unsigned int pid, unsigned long length)
{
    int allocPageNum = length % PAGE_SIZE == 0 ? length / PAGE_SIZE : length / PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //剩余内存不足
    {
        return -1;
    }
    vector<tableItem *> *pageTable = getProcessPageTable(pid); //找到进程对应的页表

    // TODO:bitmap不知道有没有存在的必要吼

    int findPage = 0;

    for (int i = 0; i < PAGE_NUM && findPage < allocPageNum; i++)
    {
        if (bitMap[i])
        {
            bitMap[i] = true;
            tableItem *item = new tableItem;
            item->pageNo = i;
            item->accessTime = 0;
            item->isChange = 0;
            item->isInMemory = false;
            item->frameNo = -1;
            item->swapAddress = 0;
            pageTable->push_back(item);
            findPage++;
        }
    }
    occupiedPageNum += allocPageNum;
    return 1;
}

/**
 * @description: 释放内存，目前暂时未想好释放内存的操作，可能只能从末尾释放
 * @param {int} pid
 * @param {int} address，释放开始地址，暂时无法实现
 * @param {int} length 单位为 B
 * @return {1为成功，否则失败}
 */
bool PageMemoryManager::memoryFree(int pid, int address, int length)
{
}

/**
 * @description: 进程结束时，释放该进程所有的内存
 * @param {int} pid
 * @return {true为成功}
 */
bool PageMemoryManager::freeAll(int pid)
{
}

/**
 * @description: 从指定地址读取一个直接
 * @param {int} pid
 * @param {int} address
 * @return {一个字节}
 */
char PageMemoryManager::accessMemory(int pid, int address)
{ //读一个字节？

    // TODO:tableitem改成指针
    // TODO:提示使用过该页
    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    int temp = address / PAGE_SIZE;
    if (temp + 1 < pageTable->size())
    {
        return 0;
    }
    tableItem *ti = pageTable->at(temp);
    //如果不在内存中
    if (!ti->isInMemory)
    {
        //如果调页失败
        if (!pageFault(pid, ti))
        {
            return 0;
        }
    }
    FrameTableItem *fti = frameTable.at(ti->frameNo);
    unsigned long realAddress = fti->getFrameAddress() + address % PAGE_SIZE;
    usePage(ti);
    return *(char *)realAddress;
}

/**
 * @description: 向内存中写入,这里假设写的东西还比较短，暂时不需要跨页写入
 * @param {unsigned long} logicalAddress  需要写入的目的地址
 * @param {void} *src  源地址
 * @param {unsigned long} size 大小
 * @param {unsigned int} pid
 * @return {true为成功}
 */
bool PageMemoryManager::write(unsigned long logicalAddress, const void *src, unsigned long size, unsigned int pid)
{
    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    int temp = (logicalAddress + size) / PAGE_SIZE;
    if (temp + 1 < pageTable->size())
    {
        return false;
    }
    tableItem *ti = pageTable->at(logicalAddress / PAGE_SIZE);
    if (!ti->isInMemory)
    {
        if (!pageFault(pid, ti))
        {
            return false;
        }
    }
    FrameTableItem *fti = frameTable.at(ti->frameNo);
    unsigned long realAddress = fti->getFrameAddress() + logicalAddress % PAGE_SIZE;
    memcpy((void *)realAddress, src, size);
    usePage(ti);
    return true;
}

/**
 * @description: 将bitMap全部设为0
 */
void PageMemoryManager::initPageTable()
{
    occupiedPageNum = 0;
    bitMap = new bool[PAGE_NUM];
    for (int i = 0; i < PAGE_NUM; i++)
    {
        bitMap[i] = false;
    }
}

/**
 * TODO:定义尚不明确
 * @description: 得到已申请的页数，不一定已申请内存
 * @param {*}
 * @return {已申请的页数}
 */
int PageMemoryManager::getOccupiedPageNum()
{
    return occupiedPageNum;
}

PageMemoryManager::PageMemoryManager()
{
    LRU_StackHead = NULL;
    LRU_StackTail = NULL;
    memory = new char[PHYSICAL_MEMORY_SIZE];
    char *pointer = memory;
    for (int i = 0; i < FRAME_NUM; i++)
    {
        FrameTableItem *fti = new FrameTableItem((unsigned long)pointer);
        frameTable.push_back(fti);
        pointer += PAGE_SIZE;
    }
    initPageTable();
}

PageMemoryManager::~PageMemoryManager()
{
    delete memory;
}

/**
 * @description: 申请进行LRU换页
 * @param {unsigned int} pid
 * @param {tableItem*} 需要放入内存的页表项
 * @return {true为成功}
 */
bool PageMemoryManager::pageFault(unsigned int pid, tableItem *ti)
{
    //首先查看是否所有的帧都被使用，倘若存在帧未被使用，则直接使用该帧即可
    for (int i = 0; i < frameTable.size(); i++)
    {
        FrameTableItem *fti = frameTable.at(i);
        if (!fti->isOccupied())
        {
            fti->setPid(pid);
            fti->setLogicalPage(ti->pageNo);
            ti->frameNo = i;
            return true;
        }
    }

    //若全部帧都被使用中，则需要通过LRU进行换页
    tableItem *temp = LRU_StackTail;

    //如果该页被锁定了，也不能换出，继续向前找
    while (temp && temp->isLock)
    {
        temp = temp->pre;
        //找了一圈又找回来了
        if (temp == LRU_StackTail)
        {
            return false;
        }
    }

    //以防出现空指针
    if (!temp)
    {
        return false;
    }

    temp->isInMemory = false;
    FrameTableItem *fti = frameTable.at(temp->frameNo);
    //被修改过，或者没有换出过，需要写入文件中
    if (temp->isChange || temp->swapAddress == 0)
    {
        unsigned long address = fileManager.write((char *)fti->getFrameAddress(), PAGE_SIZE);
        if (address != 0)
        {
            temp->isInMemory = false;
            temp->swapAddress = address;
        }
    }
    //下面将该帧给ti使用。
    //如果该页没有换出过，说明压根就没有分配内存，直接使用即可

    //否则需要换入
    if (ti->swapAddress != 0)
    {
        char *data = fileManager.readData(ti->swapAddress, PAGE_SIZE);
        if (data)
        {
            memcpy((char *)fti->getFrameAddress(), data, PAGE_SIZE);
        }
        else
        {
            return false;
        }
    }
    fti->setPid(pid);
    fti->setLogicalPage(ti->pageNo);
    ti->frameNo = temp->frameNo;
    return true;
}

/**
 * @description: 使用某一页，更新LRU使用情况
 * @param {tableItem*} ti
 * @return {*}
 */
void PageMemoryManager::usePage(tableItem *ti)
{
    if (LRU_StackHead == NULL || LRU_StackTail == NULL)
    {
        LRU_StackHead = ti;
        LRU_StackTail = ti;
    }
    else
    {
        //首先这个得存在
        if (ti)
        {
            //如果存在上一个
            if (ti->pre)
            {
                ti->pre->next = ti->next;
            }
            //如果存在下一个
            if (ti->next)
            {
                ti->next->pre = ti->pre;
            }
            ti->next = LRU_StackHead;
            if (LRU_StackHead)
            {
                LRU_StackHead->pre = ti;
            }
            LRU_StackHead = ti;
        }
    }
}