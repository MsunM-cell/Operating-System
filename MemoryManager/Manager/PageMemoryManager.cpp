/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 19:32:23
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */

#include "PageMemoryManager.h"

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

int PageMemoryManager::memoryAlloc(unsigned int pid, unsigned long length)
{
    int allocPageNum = length % PAGE_SIZE == 0 ? length / PAGE_SIZE : length / PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
    {
        return -1;
    }
    vector<tableItem *> *pageTable = getProcessPageTable(pid); //�ҵ����̶�Ӧ��ҳ��

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

bool PageMemoryManager::memoryFree(int pid, int address, int length)
{
}

bool PageMemoryManager::freeAll(int pid)
{
}

char PageMemoryManager::accessMemory(int pid, int address)
{ //��һ���ֽڣ�

    // TODO:tableitem�ĳ�ָ��
    // TODO:��ʾʹ�ù���ҳ
    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    int temp = address / PAGE_SIZE;
    if (temp + 1 < pageTable->size())
    {
        return 0;
    }
    tableItem *ti = pageTable->at(temp);
    //��������ڴ���
    if (!ti->isInMemory)
    {
        //�����ҳʧ��
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

//�����ȼ���Ҫд�Ķ����̣ܶ���ʱ�����ҳд��
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

void PageMemoryManager::initPageTable()
{
    occupiedPageNum = 0;
    bitMap = new bool[PAGE_NUM];
    for (int i = 0; i < PAGE_NUM; i++)
    {
        bitMap[i] = false;
    }
}

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

bool PageMemoryManager::pageFault(unsigned int pid, tableItem *ti)
{
    //���Ȳ鿴�Ƿ����е�֡����ʹ�ã���������֡δ��ʹ�ã���ֱ��ʹ�ø�֡����
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

    //��ȫ��֡����ʹ���У�����Ҫͨ��LRU���л�ҳ
    tableItem *temp = LRU_StackTail;

    //�����ҳ�������ˣ�Ҳ���ܻ�����������ǰ��
    while (temp && temp->isLock)
    {
        temp = temp->pre;
        //����һȦ���һ�����
        if (temp == LRU_StackTail)
        {
            return false;
        }
    }

    //�Է����ֿ�ָ��
    if (!temp)
    {
        return false;
    }

    temp->isInMemory = false;
    FrameTableItem *fti = frameTable.at(temp->frameNo);
    //���޸Ĺ�������û�л���������Ҫд���ļ���
    if (temp->isChange || temp->swapAddress == 0)
    {
        unsigned long address = fileManager.write((char *)fti->getFrameAddress(), PAGE_SIZE);
        if (address != 0)
        {
            temp->isInMemory = false;
            temp->swapAddress = address;
        }
    }
    //���潫��֡��tiʹ�á�
    //�����ҳû�л�������˵��ѹ����û�з����ڴ棬ֱ��ʹ�ü���

    //������Ҫ����
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

void PageMemoryManager::usePage(tableItem *ti)
{
    if (LRU_StackHead == NULL || LRU_StackTail == NULL)
    {
        LRU_StackHead = ti;
        LRU_StackTail = ti;
    }
    else
    {
        //��������ô���
        if (ti)
        {
            //���������һ��
            if (ti->pre)
            {
                ti->pre->next = ti->next;
            }
            //���������һ��
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