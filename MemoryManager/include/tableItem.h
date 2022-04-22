/*
 * @Date: 2022-04-08 17:19:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-14 19:46:25
 * @FilePath: \Operating-System\MemoryManager\include\tableItem.h
 */

#ifndef TABLEITEM
#define TABLEITEM

class FrameTableItem;
struct PageTableItem
{
    FrameTableItem *frame;
    long long pageNo;      // TODO:有没有必要？
    bool isInMemory;       //是否在内存中
    bool isChange;         //修改位
    long long swapAddress; //等于-1则表示不在外存中
};

#endif