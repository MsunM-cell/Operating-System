/*
 * @Date: 2022-04-01 15:17:10
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 16:24:03
 * @FilePath: \Operating-System\MemoryManager\include\PageTable.h
 */

#include <unordered_map>
#include <utility>
#include "PageTableItem.h"

typedef std::pair<unsigned long, unsigned int> pagePair;

struct hash_func
{
    unsigned long long operator()(const pagePair &pair) const
    {
        return pair.first * 9999 + pair.second;
    }
};

struct cmp_func
{
    bool operator()(const pagePair &p1, const pagePair &p2) const
    {
        return p1.first == p2.first && p1.second == p2.second;
    }
};

class PageTable
{
private:
    std::unordered_map<pagePair, PageTableItem*, hash_func, cmp_func> pageTableMap;
    PageTableItem *LRU_StackHead, *LRU_StackTail;

public:
    PageTable(/* args */);
    ~PageTable();
};

PageTable::PageTable(/* args */)
{
}

PageTable::~PageTable()
{
}
