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
    // TODO:��û�б�Ҫ��
    unsigned long pageNo;
    bool isInMemory;
    bool isChange;
    unsigned long swapAddress;
    bool isLock;
};