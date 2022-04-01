/*
 * @Date: 2022-04-01 15:07:54
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 16:08:38
 * @FilePath: \Operating-System\MemoryManager\include\ProcessTableItem.h
 */

#include <unordered_map>
#include "PageTableItem.h"
class ProcessTableItem
{
private:
    unsigned int pid;
    unsigned long pageCount;
    std::unordered_map<long, PageTableItem *> mPageTable;

public:
    
};