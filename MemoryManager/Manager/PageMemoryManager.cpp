/*
 * @Date: 2022-03-24 13:40:50
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-03-26 12:03:33
 * @FilePath: \Operating-System\MemoryManager\Manager\PageMemoryManager.cpp
 */
#include "../Interface/MemoryManager.cpp"
#include <vector>
#include <map>

using namespace std;

struct tableItem
{
    int pageNo;
    bool isSwap;
    int accessTime;
    bool isChange;
    int swapAddress;
};

class PageMemoryManager : public MemoryManager
{
private:
    int memorySize;
    int pageSize;
    int pageNum;
    bool *bitMap;
    int occupiedPageNum;
    map<int, vector<tableItem> *> tableMap;
    vector<tableItem> *getProcessPageTable(int pid)
    {
        auto iter = tableMap.find(pid);
        vector<tableItem> *pageTable;
        if (iter == tableMap.end())
        {
            pageTable = new vector<tableItem>;
            tableMap.insert(pair<int, vector<tableItem> *>(pid, pageTable));
        }
        else
        {
            pageTable = iter->second;
        }
        return pageTable;
    }

public:
    PageMemoryManager(int memorySize, int pageSize);

    ~PageMemoryManager();

    int pageAlloc(int pid, int length)
    {
        int allocPageNum = length % pageSize == 0 ? length / pageSize : length / pageSize + 1;
        if (pageNum - occupiedPageNum < allocPageNum) //剩余内存不足
        {
            return -1;
        }
        vector<tableItem> *pageTable = getProcessPageTable(pid); //找到进程对应的页表
        int findPage = 0;
        for (int i = 0; i < pageNum && findPage < allocPageNum; i++)
        {
            if (bitMap[i])
            {
                bitMap[i] = true;
                tableItem item;
                item.accessTime = 0;
                item.isChange = 0;
                item.pageNo = i;
                item.swapAddress = 0;
                pageTable->push_back(item);
                findPage++;
            }
        }
        return 1;
    }
    
    // 不太清楚是否可以中间某一页
    bool pageFree(int pid, int address, int length) 
    {
        //假如可以释放中间某一页的话，那程序的逻辑地址是否是不连续的呢？
        //这样怎么判断该程序是否越界？
        //怎么进行逻辑地址与物理地址的转换？比如说啊书上给出的转换地址方法是逻辑地址除以页表大小，得到第几个页表项，根据该页表项得到对应的物理块地址
        //假如不能从中间释放，只能从末尾释放的话，是否太过简单不方便？
        //是否引入堆区和栈区？堆区使用链表进行管理吗 
        //
    }

    bool freeAll(int pid)
    {
    }

    char accessMemory(int pid, int address)
    { //读一个字节？
        vector<tableItem>* pageTable = getProcessPageTable(pid);
        int temp = address / pageSize;
        if(temp+1 < pageTable->size()){
            return -1;
        }
        int pageNo = pageTable->at(temp).pageNo;
    }

    void initPageTable()
    {
        occupiedPageNum = 0;
        bitMap = new bool[pageNum];
        for (int i = 0; i < pageNum; i++)
        {
            bitMap[i] = false;
        }
    }

    int getOccupiedPageNum()
    {
        return occupiedPageNum;
    }
};

PageMemoryManager::PageMemoryManager(int memorySize, int pageSize)
{
    this->memorySize = memorySize;
    this->pageSize = pageSize;
    pageNum = memorySize / pageSize;
    initPageTable();
}

PageMemoryManager::~PageMemoryManager()
{
}
