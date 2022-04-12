/*
 * @Date: 2022-04-01 16:56:42
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 18:56:37
 * @FilePath: \Operating-System\MemoryManager\Manager\FrameTableItem.cpp
 */
#include "../include/FrameTableItem.h"

/**
 * @brief: 初始化帧表项
 * @param {long long} frameAddress
 * @param {unsigned int} frameNo
 * @return {*}
 */
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