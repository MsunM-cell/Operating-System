/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-23 18:09:46
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
    accessTime = 0;
    pageFaultTime = 0;
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
    instance = this;
    monitorThread = thread(monitor);
}

PageMemoryManager::~PageMemoryManager()
{
    for (auto it : tableMap)
    {
        freeProcess(it.first);
    }
    for (FrameTableItem *f : frameTable)
    {
        delete f;
    }
    delete bitMap;
    monitorThread.join();
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
int PageMemoryManager::createProcess(PCB &p)
{
    vector<tableItem *> *pageTable = new vector<tableItem *>;
    tableMap[p.id] = pageTable;
    int allocPageNum = p.size % mem_config.PAGE_SIZE == 0 ? p.size / mem_config.PAGE_SIZE : p.size / mem_config.PAGE_SIZE + 1;
    if (PAGE_NUM - occupiedPageNum < allocPageNum) //剩余内存不足
    {
        stringstream ss;
        ss << "There is no enough Memory(" << (PAGE_NUM - occupiedPageNum) * mem_config.PAGE_SIZE << ") for " << p.size;
        tableMap.erase(p.id);
        delete pageTable;
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
 * @brief 进程结束时，释放该进程所有的内存
 * @param {int} pid
 * @return {1为成功}
 */
int PageMemoryManager::freeProcess(PCB &p)
{
    MyFileManager *fileManager = MyFileManager::getInstance();
    vector<tableItem *> *mPageTable = getProcessPageTable(p.id);
    if (mPageTable == nullptr)
    {
        return -1;
    }
    for (int i = 0; i < mPageTable->size(); i++)
    {
        tableItem *ti = mPageTable->at(i);
        //如果在内存中
        if (ti->isInMemory)
        {
            FrameTableItem *fti = ti->frame;
            fti->unUsed();
            usedFrameNum--;
        }
        //如果在外存中
        else if (ti->swapAddress != -1)
        {
            char *res = fileManager->readData(ti->swapAddress, mem_config.PAGE_SIZE);
            delete res;
            swapPageNum--;
        }
        //如果根本没有分配过内存
        else
        {
        }
        bitMap[ti->pageNo] = false;
        occupiedPageNum--;
        delete ti;
    }
    mPageTable->clear();
    delete mPageTable;
    tableMap.erase(p.id);
    return 1;
}

int PageMemoryManager::freeProcess(int pid)
{
    MyFileManager *fileManager = MyFileManager::getInstance();
    vector<tableItem *> *mPageTable = getProcessPageTable(pid);
    for (int i = 0; i < mPageTable->size(); i++)
    {
        tableItem *ti = mPageTable->at(i);
        //如果在内存中
        if (ti->isInMemory)
        {
            FrameTableItem *fti = ti->frame;
            fti->unUsed();
            usedFrameNum--;
        }
        //如果在外存中
        else if (ti->swapAddress != -1)
        {
            char *res = fileManager->readData(ti->swapAddress, mem_config.PAGE_SIZE);
            delete res;
            swapPageNum--;
        }
        //如果根本没有分配过内存
        else
        {
        }
        bitMap[ti->pageNo] = false;
        occupiedPageNum--;
        delete ti;
    }
    mPageTable->clear();
    delete mPageTable;
    tableMap.erase(pid);
    return 1;
}

/**
 * @brief 从指定地址读取一个字节
 * @param {int} pid
 * @param {int} address
 * @return {一个字节}
 */
char PageMemoryManager::accessMemory(int pid, int address_index)
{ //读一个字节？

    accessTime++;
    long long address = address_index * 8;

    vector<tableItem *> *pageTable = getProcessPageTable(pid);
    if (pageTable == nullptr)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return char(-1);
    }
    int temp = address / mem_config.PAGE_SIZE;
    if (temp + 1 > pageTable->size())
    {
        stringstream ss;
        ss << "process " << pid << " asscess memory address " << address << " out of bound (" << pageTable->size() * mem_config.PAGE_SIZE << ")";
        Log::logI(TAG, ss.str());
        return char(-1);
    }
    tableItem *ti = pageTable->at(temp);
    //如果不在内存中
    if (!ti->isInMemory)
    {
        //如果调页失败
        if (!pageFault(pid, ti))
        {
            return char(-1);
        }
    }
    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + address % mem_config.PAGE_SIZE;
    useFrame(fti);
    return *(char *)realAddress;
}

/**
 * @brief 向内存中写入,这里假设写的东西还比较短，暂时不需要跨页写入
 * @param {long long} logicalAddress  需要写入的目的地址
 * @param {void} *src  源地址
 * @param {long long} size 大小
 * @param {unsigned int} pid
 * @return {1为成功}
 */
int PageMemoryManager::writeMemory(int address_index, char src, unsigned int pid)
{
    accessTime++;
    long long logicalAddress = address_index * 8;
    int size = 8;
    vector<tableItem *> *pageTable = getProcessPageTable(pid);

    if (pageTable == nullptr)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return -1;
    }

    if (logicalAddress + size > pageTable->size() * mem_config.PAGE_SIZE)
    {
        stringstream ss;
        ss << "process " << pid << " write logical address [" << logicalAddress << " - " << logicalAddress + size << "] out of bound (" << pageTable->size() * mem_config.PAGE_SIZE - 1 << ")!";
        Log::logE(TAG, ss.str());
        return -1;
    }
    tableItem *ti = pageTable->at(logicalAddress / mem_config.PAGE_SIZE);
    if (!ti->isInMemory)
    {
        if (!pageFault(pid, ti))
        {
            return -1;
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

    return 1;
}

/**
 * @brief 将bitMap全部设为0
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

    pageFaultTime++;

    stringstream logMSG;
    logMSG << "process " << pid << " access page " << ti->pageNo << " fail";
    Log::logI(TAG, logMSG.str());

    //首先查看是否所有的帧都被使用，倘若存在帧未被使用，则直接使用该帧即可
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

        long long address = MyFileManager::getInstance()->write((char *)frameTableItem->getFrameAddress(), mem_config.PAGE_SIZE);
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
void PageMemoryManager::useFrame(FrameTableItem *fti)
{

    if (!fti)
    {
        return;
    }

    //假如链表中没有帧，这个是第一个
    if (LRU_StackHead == nullptr || LRU_StackTail == nullptr)
    {
        LRU_StackHead = fti;
        LRU_StackTail = fti;
        LRU_StackHead->pre = LRU_StackTail;
        LRU_StackTail->next = LRU_StackHead;
    }
    else
    {
        //如果这个帧之前没有添加进链表过
        if (fti->pre == nullptr && fti->next == nullptr)
        {
            fti->next = LRU_StackHead;
            fti->pre = LRU_StackTail;
            LRU_StackHead->pre = fti;

            LRU_StackTail->next = fti;
            LRU_StackHead = fti;
        }
        //如果这个帧为帧头
        else if (fti == LRU_StackHead)
        {
            return;
        }
        //如果这个帧为帧尾
        else if (fti == LRU_StackTail)
        {
            LRU_StackTail = fti->pre;

            LRU_StackTail->next = fti;
            LRU_StackHead->pre = fti;

            fti->next = LRU_StackHead;
            fti->pre = LRU_StackTail;

            LRU_StackHead = fti;
        }
        //这个帧在中间的情况
        else
        {
            //如果存在上一个
            if (fti->pre)
            {
                fti->pre->next = fti->next;
            }
            //如果存在下一个
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
//  * @brief 将该进程的页全部填满1，使系统为其分配内存，以测试换页输出，仅用于测试
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
//     Log::stopLog(); //暂时将日志输出阈值调到最高，以防stuff过程中输出太多信息
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

    if (pageTable == nullptr)
    {
        stringstream ss;
        ss << "process " << pid << " not found !" << endl;
        Log::logE(TAG, ss.str());
        return -1;
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
        return -1;
    }
    in >> j;
    in.close();
    for (int i = 0; i < j["content"].size(); ++i)
    {
        string s = j["content"][i];
        s += '\0'; //末尾\0
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
        return -1;
    }
    int address_index = 0;
    for (string s : codeTemp)
    {
        for (int i = 0; i < s.size(); i++)
        {
            writeMemory(address_index++, s[i], pid);
        }
        //看起来不用补充\0了
    }
    return 1;
}

void PageMemoryManager::dms_command()
{
    cout << endl;
    long inUsed = getUsedFrameNum() * mem_config.PAGE_SIZE;
    int ratio = 100.0 * getUsedFrameNum() / mem_config.FRAME_NUM;
    long available = getPhysicalMemorySize() - inUsed;
    long committed = getOccupiedPageNum() * mem_config.PAGE_SIZE;
    cout << "In used: " << inUsed << " B Memory used rate:" << ratio << " %" << endl;
    cout << "Available: " << available << " B" << endl;
    cout << "Committed: " << committed << " / " << getLogicalMemorySize() << "  B" << endl;

    if (getAccessTime() != 0)
        cout << "total access: " << getAccessTime() << " \tpage fault times: " << getPageFaultTime() << " \tfault rate: " << (100.0 * getPageFaultTime() / getAccessTime()) << " %" << endl;
    else
    {
        cout << "total access: " << getAccessTime() << " \tpage fault times: " << getPageFaultTime() << endl;
    }

    cout << "frame use graph" << endl;
    int i = 0;
    for (FrameTableItem *item : frameTable)
    {
        if (item->isOccupied())
        {
            cout << "*" << setw(3) << item->getPid() << "  ";
        }
        else
        {
            cout << "o" << setw(3) << " "
                 << "  ";
        }
        i++;
        if (i >= 5)
        {
            i = 0;
            cout << endl;
        }
    }
    cout << endl;
    // for (auto it = tableMap.begin(); it != tableMap.end(); it++)
    // {
    //     int pid = it->first;
    //     vector<tableItem *> *pageTable = it->second;
    //     long use = 0;
    //     for (tableItem *ti : *pageTable)
    //     {
    //         if (ti->isInMemory || ti->swapAddress != -1)
    //         {
    //             use += mem_config.PAGE_SIZE;
    //         }
    //     }
    //     cout << "[pid " << pid << "] " << use << " B" << endl;
    // }
}

void PageMemoryManager::monitor()
{
    ofstream log;
    long inUsed;
    int ratio;
    long available;
    long committed;
    log.open("./MemoryManager/test/MemoryUsage.txt", ios::out);
    if (!log.is_open())
    {
        return;
    }
    log << setw(12) << left << "time"
        << " "
        << setw(10) << "inUsed"
        << " "
        << setw(10) << "ratio"
        << " "
        << setw(10) << "available"
        << " "
        << setw(10) << "committed"
        << " " << endl;
    Sleep(2000);

    PageMemoryManager *manager = PageMemoryManager::getInstance();
    while (manager)
    {
        inUsed = manager->getUsedFrameNum() * mem_config.PAGE_SIZE;
        ratio = 100.0 * manager->getUsedFrameNum() / mem_config.FRAME_NUM;
        available = manager->getPhysicalMemorySize() - inUsed;
        committed = manager->getOccupiedPageNum() * mem_config.PAGE_SIZE;

        log << setw(12) << Log::getTimeString() << " "
            << setw(10) << inUsed << " "
            << setw(10) << ratio << " "
            << setw(10) << available << " "
            << setw(10) << committed << " "
            << endl;
        Sleep(500);
    }
    log.close();
}