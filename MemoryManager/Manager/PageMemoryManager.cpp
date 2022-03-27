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
        if (pageNum - occupiedPageNum < allocPageNum) //ʣ���ڴ治��
        {
            return -1;
        }
        vector<tableItem> *pageTable = getProcessPageTable(pid); //�ҵ����̶�Ӧ��ҳ��
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
    
    // ��̫����Ƿ�����м�ĳһҳ
    bool pageFree(int pid, int address, int length) 
    {
        //��������ͷ��м�ĳһҳ�Ļ����ǳ�����߼���ַ�Ƿ��ǲ��������أ�
        //������ô�жϸó����Ƿ�Խ�磿
        //��ô�����߼���ַ�������ַ��ת��������˵�����ϸ�����ת����ַ�������߼���ַ����ҳ���С���õ��ڼ���ҳ������ݸ�ҳ����õ���Ӧ��������ַ
        //���粻�ܴ��м��ͷţ�ֻ�ܴ�ĩβ�ͷŵĻ����Ƿ�̫���򵥲����㣿
        //�Ƿ����������ջ��������ʹ��������й����� 
        //
    }

    bool freeAll(int pid)
    {
    }

    char accessMemory(int pid, int address)
    { //��һ���ֽڣ�
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
