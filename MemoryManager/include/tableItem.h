/*
 * @Date: 2022-04-08 17:19:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:19:30
 * @FilePath: \Operating-System\MemoryManager\include\tableItem.h
 */

class FrameTableItem;

struct tableItem
{
    FrameTableItem *frame;
    // TODO:有没有必要？
    unsigned long pageNo;
    bool isInMemory;
    bool isChange;
    unsigned long swapAddress;
    bool isLock;
};