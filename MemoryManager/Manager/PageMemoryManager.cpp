/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:24:13
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */

#include "../include/PageMemoryManager.h"
#include "MyFileManager.cpp"
#include "Log.cpp"

PageMemoryManager *PageMemoryManager::getInstance()
{
    if (instance)
    {
        return instance;
    }
    instance = new PageMemoryManager();
    return instance;
}

/**
 * @description: �õ�һ�����̶�Ӧ��ҳ��
 * @param {int} pid
 * @return {�ý��̶�Ӧ��ҳ��}
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
 * @description: ����pid�Լ���Ҫ����ĳ��ȣ�Ϊ�ý��̷����ڴ�
 * @param {unsigned int} pid
 * @param {unsigned long} length
 * @return {1Ϊ�ɹ�������ʧ��}
 */
int PageMemoryManager::memoryAlloc(unsigned int pid, unsigned long length)
{
    int allocPageNum = length % PAGE_SIZE == 0 ? length / PAGE_SIZE : length / PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
    {
        return -1;
    }
    vector<tableItem *> *pageTable = getProcessPageTable(pid); //�ҵ����̶�Ӧ��ҳ��

    // TODO:bitmap��֪����û�д��ڵı�Ҫ��

    int findPage = 0;

    for (int i = 0; i < PAGE_NUM && findPage < allocPageNum; i++)
    {
        if (bitMap[i])
        {
            bitMap[i] = true;
            tableItem *item = new tableItem;
            item->pageNo = i;
            item->isChange = 0;
            item->isInMemory = false;
            item->frame = nullptr;
            item->swapAddress = 0;
            pageTable->push_back(item);
            findPage++;
        }
    }
    occupiedPageNum += allocPageNum;
    return 1;
}

/**
 * @description: �ͷ��ڴ棬Ŀǰ��ʱδ����ͷ��ڴ�Ĳ���������ֻ�ܴ�ĩβ�ͷ�
 * @param {int} pid
 * @param {int} address���ͷſ�ʼ��ַ����ʱ�޷�ʵ��
 * @param {int} length ��λΪ B
 * @return {1Ϊ�ɹ�������ʧ��}
 */
bool PageMemoryManager::memoryFree(unsigned int pid, unsigned long address, unsigned long length)
{
}

/**
 * @description: ���̽���ʱ���ͷŸý������е��ڴ�
 * @param {int} pid
 * @return {trueΪ�ɹ�}
 */
bool PageMemoryManager::freeAll(unsigned int pid)
{
}

/**
 * @description: ��ָ����ַ��ȡһ��ֱ��
 * @param {int} pid
 * @param {int} address
 * @return {һ���ֽ�}
 */
char PageMemoryManager::accessMemory(unsigned int pid, unsigned long address)
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
    FrameTableItem *fti = ti->frame;
    unsigned long realAddress = fti->getFrameAddress() + address % PAGE_SIZE;
    useFrame(fti);
    return *(char *)realAddress;
}

/**
 * @description: ���ڴ���д��,�������д�Ķ������Ƚ϶̣���ʱ����Ҫ��ҳд��
 * @param {unsigned long} logicalAddress  ��Ҫд���Ŀ�ĵ�ַ
 * @param {void} *src  Դ��ַ
 * @param {unsigned long} size ��С
 * @param {unsigned int} pid
 * @return {trueΪ�ɹ�}
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
    FrameTableItem *fti = ti->frame;
    unsigned long realAddress = fti->getFrameAddress() + logicalAddress % PAGE_SIZE;
    memcpy((void *)realAddress, src, size);
    useFrame(fti);
    return true;
}

/**
 * @description: ��bitMapȫ����Ϊ0
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
 * TODO:�����в���ȷ
 * @description: �õ��������ҳ������һ���������ڴ�
 * @param {*}
 * @return {�������ҳ��}
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
 * @DEPRECATED���÷���������
 * @description: �������LRU��ҳ
 * @param {unsigned int} pid
 * @param {tableItem*} ��Ҫ�����ڴ��ҳ����
 * @return {trueΪ�ɹ�}
 */
bool PageMemoryManager::pageFault(unsigned int pid, tableItem *ti)
{
    string logMsg;
    logMsg += "����";
    logMsg += pid;
    logMsg += "��ȡ";
    logMsg += ti->pageNo;
    logMsg += "ʧ��";

    Log::dbug(TAG, logMsg);

    //���Ȳ鿴�Ƿ����е�֡����ʹ�ã���������֡δ��ʹ�ã���ֱ��ʹ�ø�֡����
    for (int i = 0; i < frameTable.size(); i++)
    {
        FrameTableItem *fti = frameTable.at(i);
        if (!fti->isOccupied())
        {
            fti->setPid(pid);
            fti->setLogicalPage(ti);
            ti->frame = fti;

            string logMsg;
            logMsg += "����";
            logMsg += ti->pageNo;
            logMsg += "ҳ�����ڴ�,λ��";
            logMsg += fti->getFrameAddress();

            Log::dbug(TAG, logMsg);
            return true;
        }
    }

    //��ȫ��֡����ʹ���У�����Ҫͨ��LRU���л�ҳ

    //��Ҫ��������֡
    FrameTableItem *frameTableItem = LRU_StackTail;

    //�����ҳ�������ˣ�Ҳ���ܻ�����������ǰ��
    while (frameTableItem && frameTableItem->isLocked())
    {
        frameTableItem = frameTableItem->pre;
        //����һȦ���һ�����
        // TODO:�����ֿ�������
        if (frameTableItem == LRU_StackTail)
        {
            return false;
        }
    }

    //�Է����ֿ�ָ������ҵ���ͷ
    if (!frameTableItem)
    {
        return false;
    }

    tableItem *oldTableItem = frameTableItem->getLogicalPage();
    oldTableItem->isInMemory = false;

    //���޸Ĺ�������û�л���������Ҫд���ļ���
    if (oldTableItem->isChange || oldTableItem->swapAddress == 0)
    {

        unsigned long address = MyFileManager::getInstance()->write((char *)frameTableItem->getFrameAddress(), PAGE_SIZE);
        if (address != 0)
        {
            oldTableItem->isInMemory = false;
            oldTableItem->swapAddress = address;

            string logMsg;
            logMsg += "����ҳ";
            logMsg += oldTableItem->pageNo;
            logMsg += "��֡λ��";
            logMsg += frameTableItem->getFrameAddress();

            Log::dbug(TAG, logMsg);
        }
    }
    //���潫��֡��tiʹ�á�
    //�����ҳû�л�������˵��ѹ����û�з����ڴ棬ֱ��ʹ�ü���

    //������Ҫ����
    if (ti->swapAddress != 0)
    {
        char *data = MyFileManager::getInstance()->readData(ti->swapAddress, PAGE_SIZE);
        if (data)
        {
            memcpy((char *)frameTableItem->getFrameAddress(), data, PAGE_SIZE);
            delete data;

            string logMsg;
            logMsg += "����ҳ";
            logMsg += ti->pageNo;
            logMsg += "��֡λ��";
            logMsg += frameTableItem->getFrameAddress();

            Log::dbug(TAG, logMsg);
        }
        else
        {
            return false;
        }
    }

    frameTableItem->setPid(pid);
    frameTableItem->setLogicalPage(ti);
    ti->frame = frameTableItem;

    return true;
}

/**
 * @description: ʹ��ĳһ֡������LRUʹ�����
 * @param {FrameTableItem*} ftiҪʹ�õ�֡
 * @return {*}
 */
void PageMemoryManager::useFrame(FrameTableItem *fti)
{
    if (LRU_StackHead == NULL || LRU_StackTail == NULL)
    {
        LRU_StackHead = fti;
        LRU_StackTail = fti;
    }
    else
    {
        //��������ô���
        if (fti)
        {
            //���������һ��
            if (fti->pre)
            {
                fti->pre->next = fti->next;
            }
            //���������һ��
            if (fti->next)
            {
                fti->next->pre = fti->pre;
            }
            fti->next = LRU_StackHead;
            if (LRU_StackHead)
            {
                LRU_StackHead->pre = fti;
            }
            LRU_StackHead = fti;
        }
    }
}

/**
 * @description: ���ý��̵�ҳȫ������1�������ڲ���
 * @param {unsigned int} pid
 * @return {*}
 */
void PageMemoryManager::stuff(unsigned int pid)
{
    vector<tableItem *> *table = getProcessPageTable(pid);
    for (tableItem *ti : *table)
    {
        if (!ti->isInMemory)
        {
            if (!pageFault(pid, ti))
            {
                continue;
            }
        }
        FrameTableItem *fti = ti->frame;
        unsigned long realAddress = fti->getFrameAddress();
        memset((void *)realAddress, 0, PAGE_SIZE);
        useFrame(fti);
    }
}