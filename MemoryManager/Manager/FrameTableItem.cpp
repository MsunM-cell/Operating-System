/*
 * @Date: 2022-04-01 16:56:42
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 17:09:39
 * @FilePath: \Operating-System\MemoryManager\Manager\FrameTableItem.cpp
 */
#include "FrameTableItem.h"

FrameTableItem::FrameTableItem(unsigned long frameAddress)
{
    this->frameAddress = frameAddress;
    this->logicalPage = 0;
    this->occupied = false;
    this->pid = 0;
}