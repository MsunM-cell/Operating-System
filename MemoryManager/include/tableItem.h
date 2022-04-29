/*
 * @Date: 2022-04-08 17:19:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-28 19:37:52
 * @FilePath: \Operating-System\MemoryManager\include\tableItem.h
 */

#ifndef TABLEITEM
#define TABLEITEM

class FrameTableItem;
struct tableItem
{
    FrameTableItem *frame;
    long long pageNo;
    bool isInMemory;
    bool isChange;
    //等于-1则表示不在外存中
    long long swapAddress;
    bool isLock;
};

#endif