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
 * @brief �õ�һ�����̶�Ӧ��ҳ��
 * @param {int} pid
 * @return {�ý��̶�Ӧ��ҳ��}
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
 * @brief ����pid�Լ���Ҫ����ĳ��ȣ�Ϊ�ý��̷����ڴ�
 * @param {unsigned int} pid
 * @param {long long} length
 * @return {1Ϊ�ɹ�������ʧ��}
 */
int PageMemoryManager::createProcess(unsigned int pid, long long length)
{
    vector<tableItem *> *pageTable = new vector<tableItem *>;
    tableMap[pid] = pageTable;
    int allocPageNum = length % PAGE_SIZE == 0 ? length / PAGE_SIZE : length / PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
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
 * @brief �ͷ��ڴ棬Ŀǰ��ʱδ����ͷ��ڴ�Ĳ���������ֻ�ܴ�ĩβ�ͷ�
 * @param {int} pid
 * @param {int} address���ͷſ�ʼ��ַ����ʱ�޷�ʵ��
 * @param {int} length ��λΪ B
 * @return {1Ϊ�ɹ�������ʧ��}
 */
bool PageMemoryManager::memoryFree(unsigned int pid, long long address, long long length)
{
    return false;
}

/**
 * @brief ���̽���ʱ���ͷŸý������е��ڴ�
 * @param {int} pid
 * @return {trueΪ�ɹ�}
 */
bool PageMemoryManager::freeAll(unsigned int pid)
{
    MyFileManager *fileManager = MyFileManager::getInstance();
    vector<tableItem *> *mPageTable = getProcessPageTable(pid);
    for (int i = 0; i < mPageTable->size(); i++)
    {
        tableItem *ti = mPageTable->at(i);
        //������ڴ���
        if (ti->isInMemory)
        {
            FrameTableItem *frameTableItem = ti->frame;
            frameTableItem->unUsed();
            usedFrameNum--;
        }
        //����������
        else if (ti->swapAddress != 0)
        {
            char *res = fileManager->readData(ti->swapAddress, PAGE_SIZE);
            delete res;
            swapPageNum--;
        }
        //�������û�з�����ڴ�
        else
        {
        }
        bitMap[ti->pageNo] = false;
        occupiedPageNum--;
    }
    return true;
}

/**
 * @brief ��ָ����ַ��ȡһ���ֽ�
 * @param {int} pid
 * @param {int} address
 * @return {һ���ֽ�}
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
    //��������ڴ���
    if (!ti->isInMemory)
    {
        //�����ҳʧ��
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
 * @brief ���ڴ���д��,�������д�Ķ������Ƚ϶̣���ʱ����Ҫ��ҳд��
 * @param {long long} logicalAddress  ��Ҫд���Ŀ�ĵ�ַ
 * @param {void} *src  Դ��ַ
 * @param {long long} size ��С
 * @param {unsigned int} pid
 * @return {trueΪ�ɹ�}
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
 * @brief ��bitMapȫ����Ϊ0
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
 * @brief �õ��������ҳ������һ���������ڴ�
 * @param {*}
 * @return {�������ҳ��}
 */
int PageMemoryManager::getOccupiedPageNum()
{
    return occupiedPageNum;
}

/**
 * @brief �������LRU��ҳ
 * @param {unsigned int} pid ����pid
 * @param {tableItem*} ��Ҫ����ҳ����
 * @return {trueΪ�ɹ�}
 */
bool PageMemoryManager::pageFault(unsigned int pid, tableItem *ti)
{

    stringstream logMSG;
    logMSG << "process " << pid << " access page " << ti->pageNo << " fail";
    Log::logI(TAG, logMSG.str());

    //���Ȳ鿴�Ƿ����е�֡����ʹ�ã���������֡δ��ʹ�ã���ֱ��ʹ�ø�֡����
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

    //��ȫ��֡����ʹ���У�����Ҫͨ��LRU���л�ҳ

    //��Ҫ��������֡
    FrameTableItem *frameTableItem = LRU_StackTail;

    //�����ҳ�������ˣ�Ҳ���ܻ�����������ǰ��
    while (frameTableItem && frameTableItem->isLocked())
    {
        frameTableItem = frameTableItem->pre;
        //����һȦ���һ�����
        if (frameTableItem == LRU_StackTail)
        {
            stringstream ss;
            ss << "there is no frame can be swaped out";
            Log::logE(TAG, ss.str());
            return false;
        }
    }

    //�Է����ֿ�ָ������ҵ���ͷ
    if (!frameTableItem)
    {
        stringstream ss;
        ss << "unexpected frame table item null pointer";
        Log::logE(TAG, ss.str());
        return false;
    }

    tableItem *oldTableItem = frameTableItem->getLogicalPage();
    oldTableItem->isInMemory = false;

    //���޸Ĺ�������û�л���������Ҫд���ļ���
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
    //���潫��֡��tiʹ�á�
    //�����ҳû�л�������˵��ѹ����û�з����ڴ棬ֱ��ʹ�ü���

    //������Ҫ����
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
        //ֱ�ӽ���һ֡�����tiʹ��
        // TODO:�Ͻ�һ����Ҫ����һҳԭ������ȫ����䣬���ⱻ�������̶�ȡ
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
 * @brief ʹ��ĳһ֡������֡�ᵽ˫��LRU�����ͷ��,��,��������ѭ����(...)
 * //TODO:��ѭ���ĵ�
 * @param {FrameTableItem*} ftiҪʹ�õ�֡
 * @return {*}
 */
void PageMemoryManager::useFrame(FrameTableItem *frameTableItem)
{

    if (!frameTableItem)
    {
        return;
    }

    //����������û��֡������ǵ�һ��
    if (LRU_StackHead == nullptr || LRU_StackTail == nullptr)
    {
        LRU_StackHead = frameTableItem;
        LRU_StackTail = frameTableItem;
        LRU_StackHead->pre = LRU_StackTail;
        LRU_StackTail->next = LRU_StackHead;
    }
    else
    {
        //������֮֡ǰû����ӽ������
        if (frameTableItem->pre == nullptr && frameTableItem->next == nullptr)
        {
            frameTableItem->next = LRU_StackHead;
            frameTableItem->pre = LRU_StackTail;
            LRU_StackHead->pre = frameTableItem;

            LRU_StackTail->next = frameTableItem;
            LRU_StackHead = frameTableItem;
        }
        //������֡Ϊ֡ͷ
        else if (frameTableItem == LRU_StackHead)
        {
            return;
        }
        //������֡Ϊ֡β
        else if (frameTableItem == LRU_StackTail)
        {
            LRU_StackTail = frameTableItem->pre;

            LRU_StackTail->next = frameTableItem;
            LRU_StackHead->pre = frameTableItem;

            frameTableItem->next = LRU_StackHead;
            frameTableItem->pre = LRU_StackTail;

            LRU_StackHead = frameTableItem;
        }
        //���֡���м�����
        else
        {
            //���������һ��
            if (frameTableItem->pre)
            {
                frameTableItem->pre->next = frameTableItem->next;
            }
            //���������һ��
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
 * @brief ���ý��̵�ҳȫ������1��ʹϵͳΪ������ڴ棬�Բ��Ի�ҳ����������ڲ���
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
    Log::stopLog(); //��ʱ����־�����ֵ������ߣ��Է�stuff���������̫����Ϣ
    vector<tableItem *> *table = getProcessPageTable(pid);
    for (int i = 0; i < table->size(); i++)
    {
        write(i * PAGE_SIZE, src, PAGE_SIZE, pid);
    }
    Log::continueLog();
    delete src;
}