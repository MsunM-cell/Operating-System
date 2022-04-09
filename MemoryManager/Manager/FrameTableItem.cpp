/*
 * @Date: 2022-04-01 16:56:42
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 13:50:19
 * @FilePath: \Operating-System\MemoryManager\Manager\FrameTableItem.cpp
 */
#include "../include/FrameTableItem.h"

FrameTableItem::FrameTableItem(long long frameAddress,unsigned int frameNo)
{
    this->frameAddress = frameAddress;
    this->logicalPage = 0;
    this->occupied = false;
    this->pid = 0;
    this->pre = nullptr;
    this->next = nullptr;
    this->locked = false;
    this->frameNo = frameNo;
}