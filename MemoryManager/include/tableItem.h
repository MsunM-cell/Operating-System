/*
 * @Date: 2022-04-08 17:19:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:52:02
 * @FilePath: \Operating-System\MemoryManager\include\tableItem.h
 */

#ifndef TABLEITEM
#define TABLEITEM

class FrameTableItem;
struct tableItem
{
    FrameTableItem *frame;
    // TODO:��û�б�Ҫ��
    long long pageNo;
    bool isInMemory;
    bool isChange;
    long long swapAddress;
    bool isLock;
};

#endif