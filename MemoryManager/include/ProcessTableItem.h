/*
 * @Date: 2022-04-01 15:07:54
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:34:05
 * @FilePath: \Operating-System\MemoryManager\include\ProcessTableItem.h
 */

#include <unordered_map>
#include "PageTableItem.h"
class ProcessTableItem
{
private:
    unsigned int pid;
    long long pageCount;
    std::unordered_map<long, PageTableItem *> mPageTable;

public:
    
};