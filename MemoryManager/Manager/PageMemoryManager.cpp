/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-18 17:58:12
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */

#include "../include/PageMemoryManager.h"
#include "../Entity/MyFileManager.cpp"
#include "../Entity/Log.cpp"

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

PageMemoryManager::~PageMemoryManager()
{
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
int PageMemoryManager::createProcess(PCB &p)
{
    vector<tableItem *> *pageTable = new vector<tableItem *>;
    tableMap[p.id] = pageTable;
    int allocPageNum = p.size % mem_config.PAGE_SIZE == 0 ? p.size / mem_config.PAGE_SIZE : p.size / mem_config.PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
    {
        stringstream ss;
        ss << "There is no enough Memory(" << (PAGE_NUM - occupiedPageNum) * mem_config.PAGE_SIZE << ") for " << p.size;
        Log::logE(TAG, ss.str());
        return -1;
    }

    int findPage = 0;

    stringstream ss;
    ss << "process " << p.id << " ask for " << allocPageNum << " pages";
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
    int load_res = load_ins(p.id, p.path);
    occupiedPageNum += allocPageNum;
    if (load_res == 0)
    {
        freeProcess(p);
        return -1;
    }
    return 1;
}

/**
 * @brief ���̽���ʱ���ͷŸý������е��ڴ�
 * @param {int} pid
 * @return {1Ϊ�ɹ�}
 */
int PageMemoryManager::freeProcess(PCB &p)
{
    cout << "free Mem successfully" << endl;
    MyFileManager *fileManager = MyFileManager::getInstance();
    vector<tableItem *> *mPageTable = getProcessPageTable(p.id);
    for (int i = 0; i < mPageTable->size(); i++)
    {
        tableItem *ti = mPageTable->at(i);
        //������ڴ���
        if (ti->isInMemory)
        {
            FrameTableItem *fti = ti->frame;
            fti->unUsed();
            usedFrameNum--;
        }
        //����������
        else if (ti->swapAddress != 0)
        {
            char *res = fileManager->readData(ti->swapAddress, mem_config.PAGE_SIZE);
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
    return 1;
}

/**
 * @brief ��ָ����ַ��ȡһ���ֽ�
 * @param {int} pid
 * @param {int} address
 * @return {һ���ֽ�}
 */
char PageMemoryManager::accessMemory(int pid, int address_index)
{ //��һ���ֽڣ�

    long long address = address_index * 8;

    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    if (!pageTable)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }
    int temp = address / mem_config.PAGE_SIZE;
    if (temp + 1 > pageTable->size())
    {
        stringstream ss;
        ss << "process " << pid << " asscess memory address " << address << " out of bound (" << pageTable->size() * mem_config.PAGE_SIZE << ")";
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
    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + address % mem_config.PAGE_SIZE;
    useFrame(fti);
    return *(char *)realAddress;
}

/**
 * @brief ���ڴ���д��,�������д�Ķ������Ƚ϶̣���ʱ����Ҫ��ҳд��
 * @param {long long} logicalAddress  ��Ҫд���Ŀ�ĵ�ַ
 * @param {void} *src  Դ��ַ
 * @param {long long} size ��С
 * @param {unsigned int} pid
 * @return {1Ϊ�ɹ�}
 */
int PageMemoryManager::writeMemory(int address_index, char src, unsigned int pid)
{
    long long logicalAddress = address_index * 8;
    int size = 8;
    vector<tableItem *> *pageTable = getProcessPageTable(pid);

    if (!pageTable)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }

    if (logicalAddress + size > pageTable->size() * mem_config.PAGE_SIZE)
    {
        stringstream ss;
        ss << "process " << pid << " write logical address [" << logicalAddress << " - " << logicalAddress + size << "] out of bound (" << pageTable->size() * mem_config.PAGE_SIZE - 1 << ")!";
        Log::logE(TAG, ss.str());
        return false;
    }
    tableItem *ti = pageTable->at(logicalAddress / mem_config.PAGE_SIZE);
    if (!ti->isInMemory)
    {
        if (!pageFault(pid, ti))
        {
            return false;
        }
    }

    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + logicalAddress % mem_config.PAGE_SIZE;
    ((char *)realAddress)[0] = src;
    useFrame(fti);
    ti->isChange = true;

    stringstream logMsg;
    logMsg << "process " << pid << " write " << size / 8 << " words into page " << ti->pageNo;
    Log::logV(TAG, logMsg.str());

    return true;
}

/**
 * @brief ��bitMapȫ����Ϊ0
 */
void PageMemoryManager::initPageTable()
{
    occupiedPageNum = 0;
    bitMap = new bool[4096];
    for (int i = 0; i < 4096; i++)
    {
        bitMap[i] = false;
    }
}

/**
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
        FrameTableItem *fti = frameTable.at(i);
        if (!fti->isOccupied())
        {
            fti->setPid(pid);
            fti->setLogicalPage(ti);
            fti->setOccupied();
            usedFrameNum++;
            ti->frame = fti;
            ti->isInMemory = true;

            stringstream ss;
            ss << "move page " << ti->pageNo << " into memory , frame number " << fti->getFrameNo();
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

        long long address = MyFileManager::getInstance()->write((char *)frameTableItem->getFrameAddress(), mem_config.PAGE_SIZE);
        if (address != 0)
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
        char *data = MyFileManager::getInstance()->readData(ti->swapAddress, mem_config.PAGE_SIZE);
        if (data)
        {

            memcpy((char *)frameTableItem->getFrameAddress(), data, mem_config.PAGE_SIZE);
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
 * @brief ʹ��ĳһ֡������֡�ᵽ˫��LRU������ͷ��,��,��������ѭ����(...)
 * //TODO:��ѭ���ĵ�
 * @param {FrameTableItem*} ftiҪʹ�õ�֡
 * @return {*}
 */
void PageMemoryManager::useFrame(FrameTableItem *fti)
{

    if (!fti)
    {
        return;
    }

    //����������û��֡������ǵ�һ��
    if (LRU_StackHead == nullptr || LRU_StackTail == nullptr)
    {
        LRU_StackHead = fti;
        LRU_StackTail = fti;
        LRU_StackHead->pre = LRU_StackTail;
        LRU_StackTail->next = LRU_StackHead;
    }
    else
    {
        //������֮֡ǰû�����ӽ�������
        if (fti->pre == nullptr && fti->next == nullptr)
        {
            fti->next = LRU_StackHead;
            fti->pre = LRU_StackTail;
            LRU_StackHead->pre = fti;

            LRU_StackTail->next = fti;
            LRU_StackHead = fti;
        }
        //������֡Ϊ֡ͷ
        else if (fti == LRU_StackHead)
        {
            return;
        }
        //������֡Ϊ֡β
        else if (fti == LRU_StackTail)
        {
            LRU_StackTail = fti->pre;

            LRU_StackTail->next = fti;
            LRU_StackHead->pre = fti;

            fti->next = LRU_StackHead;
            fti->pre = LRU_StackTail;

            LRU_StackHead = fti;
        }
        //���֡���м�����
        else
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

// /**
//  * @brief ���ý��̵�ҳȫ������1��ʹϵͳΪ������ڴ棬�Բ��Ի�ҳ����������ڲ���
//  *
//  * @param {unsigned int} pid
//  * @return {*}
//  */
// void PageMemoryManager::stuff(unsigned int pid)
// {
//     char *src = new char[mem_config.PAGE_SIZE];
//     char c = -1;
//     for (int i = 0; i < mem_config.PAGE_SIZE; i++)
//     {
//         src[i] = c;
//     }
//     Log::stopLog(); //��ʱ����־�����ֵ������ߣ��Է�stuff���������̫����Ϣ
//     vector<tableItem *> *table = getProcessPageTable(pid);
//     for (int i = 0; i < table->size(); i++)
//     {
//         writeMemory(i * mem_config.PAGE_SIZE, src, mem_config.PAGE_SIZE, pid);
//     }
//     Log::continueLog();
//     delete src;
// }

int PageMemoryManager::load_ins(int pid, string file_address)
{
    vector<tableItem *> *pageTable = getProcessPageTable(pid);

    if (!pageTable)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }
    vector<string> codeTemp;
    int code_length = 0;
    json j;

    ifstream in(file_address, ios::binary);
    if (!in.is_open())
    {
        stringstream ss;
        ss << "Error opening file" << file_address << endl;
        Log::logE(TAG, ss.str());
        exit(1);
    }
    in >> j;
    in.close();
    for (int i = 0; i < j["content"].size(); ++i)
    {
        string s = j["content"][i];
        s += '\0'; //ĩβ\0
        codeTemp.push_back(s);
        code_length += s.size();
        // s = s.substr(1, s.size() - 2);
        // sprintf(memory, "%s", s.c_str());
    }

    if (code_length > pageTable->size() * mem_config.PAGE_SIZE)
    {
        stringstream ss;
        ss << "the code is too long to load in memory" << endl;
        Log::logE(TAG, ss.str());
        return 0;
    }
    int address_index = 0;
    for (string s : codeTemp)
    {
        for (int i = 0; i < s.size(); i++)
        {
            writeMemory(address_index++, s[i], pid);
        }
        //���������ò���\0��
    }

    return 1;
}