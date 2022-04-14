/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-14 17:11:21
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */

#include "../include/PageMemoryManager.h"
#include "MyFileManager.cpp"
#include "Log.cpp"

PageMemoryManager *PageMemoryManager::instance = nullptr;

PageMemoryManager *PageMemoryManager::getInstance()
{
    if (instance)
    {
        return instance;
    }
    instance = new PageMemoryManager();
    return instance;
}

PageMemoryManager::PageMemoryManager()
{
    LRU_StackHead = nullptr;
    LRU_StackTail = nullptr;
    memory = new char[PHYSICAL_MEMORY_SIZE];
    char *pointer = memory;
    for (int i = 0; i < FRAME_NUM; i++)
    {
        FrameTableItem *frameTableItem = new FrameTableItem((long long)pointer, i);
        frameTable.push_back(frameTableItem);
        pointer += PAGE_SIZE;
    }
    usedFrameNum = 0;
    swapPageNum = 0;
    initPageTable();
}

PageMemoryManager::~PageMemoryManager()
{
    delete memory;
}

/**
 * @brief 得到一个进程对应的页表
 * @param {int} pid
 * @return {该进程对应的页表}
 */
vector<tableItem *> *PageMemoryManager::getProcessPageTable(int pid)
{
    auto iter = tableMap.find(pid);
    vector<tableItem *> *pageTable;
    if (iter == tableMap.end())
    {
        return nullptr;
        // pageTable = new vector<tableItem *>;
        // tableMap[pid] = pageTable;
        // // tableMap.insert(pair<int, vector<tableItem *> *>(pid, pageTable));
    }
    else
    {
        pageTable = iter->second;
    }
    return pageTable;
}

/**
 * @brief 给定pid以及需要分配的长度，为该进程分配内存
 * @param {unsigned int} pid
 * @param {long long} length
 * @return {1为成功，否则失败}
 */
int PageMemoryManager::createProcess(unsigned int pid, long long length)
{
    vector<tableItem *> *pageTable = new vector<tableItem *>;
    tableMap[pid] = pageTable;
    int allocPageNum = length % PAGE_SIZE == 0 ? length / PAGE_SIZE : length / PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //剩余内存不足
    {
        stringstream ss;
        ss << "There is no enough Memory(" << (PAGE_NUM - occupiedPageNum) * PAGE_SIZE << ") for " << length;
        Log::logE(TAG, ss.str());
        return -1;
    }


    int findPage = 0;

    stringstream ss;
    ss << "process " << pid << " ask for " << allocPageNum << " pages";
    Log::logV(TAG, ss.str());

    for (int i = 0; i < PAGE_NUM && findPage < allocPageNum; i++)
    {
        if (!bitMap[i])
        {
            bitMap[i] = true;
            tableItem *item = new tableItem;
            item->pageNo = i;
            item->isChange = 0;
            item->isInMemory = false;
            item->frame = nullptr;
            item->swapAddress = -1;
            pageTable->push_back(item);
            findPage++;
        }
    }
    occupiedPageNum += allocPageNum;
    return 1;
}

/**
 * @brief 释放内存，目前暂时未想好释放内存的操作，可能只能从末尾释放
 * @param {int} pid
 * @param {int} address，释放开始地址，暂时无法实现
 * @param {int} length 单位为 B
 * @return {1为成功，否则失败}
 */
bool PageMemoryManager::memoryFree(unsigned int pid, long long address, long long length)
{
    return false;
}

/**
 * @brief 进程结束时，释放该进程所有的内存
 * @param {int} pid
 * @return {true为成功}
 */
bool PageMemoryManager::freeAll(unsigned int pid)
{
    MyFileManager *fileManager = MyFileManager::getInstance();
    vector<tableItem *> *mPageTable = getProcessPageTable(pid);
    for (int i = 0; i < mPageTable->size(); i++)
    {
        tableItem *ti = mPageTable->at(i);
        //如果在内存中
        if (ti->isInMemory)
        {
            FrameTableItem *frameTableItem = ti->frame;
            frameTableItem->unUsed();
            usedFrameNum--;
        }
        //如果在外存中
        else if (ti->swapAddress != 0)
        {
            char *res = fileManager->readData(ti->swapAddress, PAGE_SIZE);
            delete res;
            swapPageNum--;
        }
        //如果根本没有分配过内存
        else
        {
        }
        bitMap[ti->pageNo] = false;
        occupiedPageNum--;
    }
    return true;
}

/**
 * @brief 从指定地址读取一个字节
 * @param {int} pid
 * @param {int} address
 * @return {一个字节}
 */
char PageMemoryManager::accessMemory(unsigned int pid, long long address)
{ 
    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    if (!pageTable)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }
    int temp = address / PAGE_SIZE;
    if (temp + 1 > pageTable->size())
    {
        stringstream ss;
        ss << "process " << pid << " asscess memory address " << address << " out of bound (" << pageTable->size() * PAGE_SIZE << ")";
        Log::logI(TAG, ss.str());
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
    FrameTableItem *frameTableItem = ti->frame;
    long long realAddress = frameTableItem->getFrameAddress() + address % PAGE_SIZE;
    useFrame(frameTableItem);
    return *(char *)realAddress;
}

/**
 * @brief 向内存中写入,这里假设写的东西还比较短，暂时不需要跨页写入
 * @param {long long} logicalAddress  需要写入的目的地址
 * @param {void} *src  源地址
 * @param {long long} size 大小
 * @param {unsigned int} pid
 * @return {true为成功}
 */
bool PageMemoryManager::write(long long logicalAddress, const void *src, long long size, unsigned int pid)
{

    vector<tableItem *> *pageTable = getProcessPageTable(pid);

    if (!pageTable)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }
    
    if (logicalAddress + size > pageTable->size() * PAGE_SIZE)
    {
        stringstream ss;
        ss << "process " << pid << " write logical address [" << logicalAddress << " - " << logicalAddress + size << "] out of bound (" << pageTable->size() * PAGE_SIZE - 1 << ")!";
        Log::logE(TAG, ss.str());
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

    FrameTableItem *frameTableItem = ti->frame;
    long long realAddress = frameTableItem->getFrameAddress() + logicalAddress % PAGE_SIZE;
    memcpy((void *)realAddress, src, size);
    useFrame(frameTableItem);
    ti->isChange = true;

    stringstream logMsg;
    logMsg << "process " << pid << " write " << size << " words into page " << ti->pageNo;
    Log::logI(TAG, logMsg.str());

    return true;
}

/**
 * @brief 将bitMap全部设为0
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
 * @brief 得到已申请的页数，不一定已申请内存
 * @param {*}
 * @return {已申请的页数}
 */
int PageMemoryManager::getOccupiedPageNum()
{
    return occupiedPageNum;
}

/**
 * @brief 申请进行LRU换页
 * @param {unsigned int} pid 进程pid
 * @param {tableItem*} 需要访问页表项
 * @return {true为成功}
 */
bool PageMemoryManager::pageFault(unsigned int pid, tableItem *ti)
{

    stringstream logMSG;
    logMSG << "process " << pid << " access page " << ti->pageNo << " fail";
    Log::logI(TAG, logMSG.str());

    //首先查看是否所有的帧都被使用，倘若存在帧未被使用，则直接使用该帧即可
    for (int i = 0; i < frameTable.size(); i++)
    {
        FrameTableItem *frameTableItem = frameTable.at(i);
        if (!frameTableItem->isOccupied())
        {
            frameTableItem->setPid(pid);
            frameTableItem->setLogicalPage(ti);
            frameTableItem->setOccupied();
            usedFrameNum++;
            ti->frame = frameTableItem;
            ti->isInMemory = true;

            stringstream ss;
            ss << "move page " << ti->pageNo << " into memory , frame number " << frameTableItem->getFrameNo();
            Log::logI(TAG, ss.str());

            return true;
        }
    }

    //若全部帧都被使用中，则需要通过LRU进行换页

    //需要被换出的帧
    FrameTableItem *frameTableItem = LRU_StackTail;

    //如果该页被锁定了，也不能换出，继续向前找
    while (frameTableItem && frameTableItem->isLocked())
    {
        frameTableItem = frameTableItem->pre;
        //找了一圈又找回来了
        if (frameTableItem == LRU_StackTail)
        {
            stringstream ss;
            ss << "there is no frame can be swaped out";
            Log::logE(TAG, ss.str());
            return false;
        }
    }

    //以防出现空指针或者找到尽头
    if (!frameTableItem)
    {
        stringstream ss;
        ss << "unexpected frame table item null pointer";
        Log::logE(TAG, ss.str());
        return false;
    }

    tableItem *oldTableItem = frameTableItem->getLogicalPage();
    oldTableItem->isInMemory = false;

    //被修改过，或者没有换出过，需要写入文件中
    if (oldTableItem->isChange || oldTableItem->swapAddress == -1)
    {

        long long address = MyFileManager::getInstance()->write((char *)frameTableItem->getFrameAddress(), PAGE_SIZE);
        if (address != -1)
        {
            oldTableItem->isInMemory = false;
            oldTableItem->swapAddress = address;

            stringstream ss;
            ss << "swap out page " << oldTableItem->pageNo << " ,frame number " << frameTableItem->getFrameNo();
            swapPageNum++;
            Log::logI(TAG, ss.str());
        }
    }
    //下面将该帧给ti使用。
    //如果该页没有换出过，说明压根就没有分配内存，直接使用即可

    //否则需要换入
    if (ti->swapAddress != -1)
    {
        char *data = MyFileManager::getInstance()->readData(ti->swapAddress, PAGE_SIZE);
        if (data)
        {

            memcpy((char *)frameTableItem->getFrameAddress(), data, PAGE_SIZE);
            delete data;
            swapPageNum--;

            stringstream ss;
            ss << "swap in page " << ti->pageNo << " , frame number " << frameTableItem->getFrameNo();
            Log::logI(TAG, ss.str());
        }
        else
        {
            stringstream ss;
            ss << "CAN'T read swap page from storage !";
            Log::logE(TAG, ss.str());
            return false;
        }
    }
    else
    {
        //直接将这一帧分配给ti使用
        // TODO:严谨一点需要将这一页原有内容全部填充，以免被其他进程读取
        stringstream ss;
        ss << "Page " << ti->pageNo << " use frame " << frameTableItem->getFrameNo() << " directly";
        Log::logI(TAG, ss.str());
    }

    frameTableItem->setPid(pid);
    frameTableItem->setLogicalPage(ti);
    ti->frame = frameTableItem;
    ti->isInMemory = true;
    ti->swapAddress = -1;
    useFrame(frameTableItem);
    return true;
}

/**
 * @brief 使用某一帧，将该帧提到双向LRU链表的头部,嗯,甚至还是循环的(...)
 * //TODO:把循环改掉
 * @param {FrameTableItem*} fti要使用的帧
 * @return {*}
 */
void PageMemoryManager::useFrame(FrameTableItem *frameTableItem)
{

    if (!frameTableItem)
    {
        return;
    }

    //假如链表中没有帧，这个是第一个
    if (LRU_StackHead == nullptr || LRU_StackTail == nullptr)
    {
        LRU_StackHead = frameTableItem;
        LRU_StackTail = frameTableItem;
        LRU_StackHead->pre = LRU_StackTail;
        LRU_StackTail->next = LRU_StackHead;
    }
    else
    {
        //如果这个帧之前没有添加进链表过
        if (frameTableItem->pre == nullptr && frameTableItem->next == nullptr)
        {
            frameTableItem->next = LRU_StackHead;
            frameTableItem->pre = LRU_StackTail;
            LRU_StackHead->pre = frameTableItem;

            LRU_StackTail->next = frameTableItem;
            LRU_StackHead = frameTableItem;
        }
        //如果这个帧为帧头
        else if (frameTableItem == LRU_StackHead)
        {
            return;
        }
        //如果这个帧为帧尾
        else if (frameTableItem == LRU_StackTail)
        {
            LRU_StackTail = frameTableItem->pre;

            LRU_StackTail->next = frameTableItem;
            LRU_StackHead->pre = frameTableItem;

            frameTableItem->next = LRU_StackHead;
            frameTableItem->pre = LRU_StackTail;

            LRU_StackHead = frameTableItem;
        }
        //这个帧在中间的情况
        else
        {
            //如果存在上一个
            if (frameTableItem->pre)
            {
                frameTableItem->pre->next = frameTableItem->next;
            }
            //如果存在下一个
            if (frameTableItem->next)
            {
                frameTableItem->next->pre = frameTableItem->pre;
            }

            frameTableItem->next = LRU_StackHead;
            if (LRU_StackHead)
            {
                LRU_StackHead->pre = frameTableItem;
            }
            LRU_StackHead = frameTableItem;
        }
    }
}

/**
 * @brief 将该进程的页全部填满1，使系统为其分配内存，以测试换页输出，仅用于测试
 *
 * @param {unsigned int} pid
 * @return {*}
 */
void PageMemoryManager::stuff(unsigned int pid)
{
    char *src = new char[PAGE_SIZE];
    char c = -1;
    for (int i = 0; i < PAGE_SIZE; i++)
    {
        src[i] = c;
    }
    Log::stopLog(); //暂时将日志输出阈值调到最高，以防stuff过程中输出太多信息
    vector<tableItem *> *table = getProcessPageTable(pid);
    for (int i = 0; i < table->size(); i++)
    {
        write(i * PAGE_SIZE, src, PAGE_SIZE, pid);
    }
    Log::continueLog();
    delete src;
}