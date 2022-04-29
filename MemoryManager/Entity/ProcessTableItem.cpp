/*
 * @Date: 2022-04-28 20:01:22
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-29 11:44:03
 * @FilePath: \Operating-System\MemoryManager\Entity\ProcessTableItem.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
#include "../include/ProcessTableItem.h"

ProcessTableItem::ProcessTableItem(int pid, long long size, long long codeLength)
{
    insertPage();
    pageCount = 0;
    codeStart = 0;
    if (codeLength % WORD_SIZE != 0)
    {
        codeLength += (WORD_SIZE - codeLength % WORD_SIZE); //对齐哦
    }

    if (size % WORD_SIZE != 0)
    {
        size += (WORD_SIZE - size % WORD_SIZE);
    }

    codeEnd = codeStart + codeLength;

    dataStart = codeEnd;
    dataEnd = dataStart + size;
    stackBase = LOGICAL_SPACE - 1 << 20 - 1;
    heapBase = dataEnd;
    brk = heapBase;
    rsp = stackBase;
    initHeap();
}

void ProcessTableItem::insertPage()
{
    int page = LOGICAL_SPACE / mem_config.PAGE_SIZE;
    for (int i = 0; i < page; i++)
    {
        tableItem *pti = new tableItem;
        pti->frame = nullptr;
        pti->isChange = false;
        pti->isInMemory = false;
        pti->isLock = false;
        pti->pageNo = i;
        pti->swapAddress = -1;
        pageTable.push_back(pti);
    }
}

void ProcessTableItem::initHeap()
{
    long long heapBound = stackBase - 8 << 10; //这是栈的最大地址的下一格
    heapBound -= (WORD_SIZE - 1);              //大端法？需要从小地址开始写数据？
    /*
    ->  8 9 10 11  ---堆终止      箭头表示读一个字应该从小地址开始读
        4 5 6 7                  长度计算方法为8-0-wordsize，因为上面已经减过一次了
    ->  0 1 2 3    ---堆起始     长度为4，在0.1.2.3块上写4 ，在8.9.10.11快上也写4，访问下一块的时候0+4+2*Word Size
     */
    long freeSpace = heapBound - heapBase - WORD_SIZE;

    //因为长度加起来总是8的倍数，所以后三位可以充分利用。后三位为000则表示未分配 ， 001表示已分配
    writeWord(heapBase, freeSpace);
    writeWord(heapBound, freeSpace);
}

bool ProcessTableItem::writeWord(long long address, long word)
{
    int pageNo = address / mem_config.PAGE_SIZE;
    tableItem *ti = pageTable.at(pageNo);
    if (!ti->isInMemory)
    {
        bool pageFaultRes = PageMemoryManager::getInstance()->pageFault(pid, ti);
        if (!pageFaultRes)
        {
            return false;
        }
    }
    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + address % mem_config.PAGE_SIZE;
    memcpy((void *)address, &word, WORD_SIZE);
    PageMemoryManager::getInstance()->useFrame(fti);
    return true;
}

bool ProcessTableItem::writeChar(long long address, char c)
{

    return false;
}

// first Fit
long long ProcessTableItem::allocate(long size)
{
    if (size <= 0)
    {
        return -1;
    }
    long long pointer = heapBase;
    do
    {
        long blockSize = getBlockLength(pointer);
        if (blockSize >= size) // blockSize不包含头尾8个字节的长度
        {
            if (isBlockFree(pointer))
            {
                if (size % WORD_SIZE != 0)
                {
                    size += (WORD_SIZE - size % WORD_SIZE); //对齐，申请的长度一定为字的倍数
                }

                long leftSpaceStart = pointer + size + 2 * WORD_SIZE; //分割完这一大块block后剩余的一小块起止
                long leftSpaceEnd = pointer + blockSize + WORD_SIZE;
                if (leftSpaceEnd == leftSpaceStart || leftSpaceEnd - leftSpaceStart == WORD_SIZE)
                {
                    //如果分割完这一大块的话，剩下的一小块空间无法添加脚注，那就把剩下那一小块空间也给你吧
                    if (leftSpaceEnd == leftSpaceStart)
                        size += WORD_SIZE;
                    else if (leftSpaceEnd - leftSpaceStart == WORD_SIZE)
                    {
                        size += 2 * WORD_SIZE;
                    }
                }

                long spaceSize = size | 1;                        //让最后一位为1说明这一块被占用
                writeWord(pointer, spaceSize);                    //块首
                writeWord(pointer + size + WORD_SIZE, spaceSize); //块尾部

                if (size == blockSize)
                {
                    //全部用完啦，不用计算碎片块了
                }
                else
                {
                    long leftSpaceStart1 = pointer + size + 2 * WORD_SIZE; //重新计算碎片块的起止地址
                    long leftSpaceEnd1 = pointer + blockSize + WORD_SIZE;

                    long leftSpaceSize = leftSpaceEnd1 - leftSpaceStart1 - WORD_SIZE;
                    writeWord(leftSpaceStart1, leftSpaceSize);
                    writeWord(leftSpaceStart1, leftSpaceSize);
                }

                return pointer + WORD_SIZE; //跳过段首
            }
        }
        pointer += blockSize + 2 * WORD_SIZE; //不大于或被占用就看下一块
    } while (pointer < stackBase - 8 << 10);  //不要越到栈区了
    return -1;
}

bool ProcessTableItem::freeSpace(long long address)
{
    //首先检查释放的地址是否在堆区
    long long heapBound = stackBase - 8 << 10 - WORD_SIZE + 1;
    if (address <= heapBound - WORD_SIZE || address >= heapBase + WORD_SIZE) //应在首尾两个脚注之间
    {
        long long blockStart = address - WORD_SIZE; //如果传入的地址没使坏的话（比如从段中间开始释放），减一个字就能得到段首脚注了
        if (isBlockFree(blockStart))
        {
            cout << "尝试释放未申请空间！" << endl;
            return false;
        }
        long size = getBlockLength(blockStart);

        long blockEnd = blockStart + WORD_SIZE + size;

        //释放一段空间后应该有4种情况，1.前后均为占用中的块  2.前面为空闲块   3.后面为空闲块  4.前后均为空闲块
        long long previousBlock = blockStart + size + 2 * WORD_SIZE;
        long long nextBlock = blockStart - WORD_SIZE;

        if (previousBlock >= heapBound || !isBlockFree(previousBlock)) //没有上一块或上一块被占用
        {
            //第一种情况，上下均不用合并
            if (nextBlock < heapBase || !isBlockFree(nextBlock))
            {
                writeWord(blockStart, size);
                writeWord(blockEnd, size);
            }
            //第3种情况，后面的需要合并
            else
            {
                long nextBlockSize = getBlockLength(nextBlock);
                long long nextBlockStart = nextBlock - nextBlockSize - WORD_SIZE;
                long addupBlockSize = nextBlockSize + size + 2 * WORD_SIZE; //省下来中间两个脚注的空间
                writeWord(nextBlockStart, addupBlockSize);
                writeWord(blockEnd, addupBlockSize);
            }
        }

        else //前面块需要合并
        {
            //没有后面块或被占用，第2种情况，只需合并前一个
            if (nextBlock < heapBase || !isBlockFree(nextBlock))
            {
                long preBlockSize = getBlockLength(previousBlock);
                long long preBlockEnd = previousBlock + preBlockSize + WORD_SIZE;
                long addupBlockSize = preBlockSize + size + 2 * WORD_SIZE;
                writeWord(blockStart, addupBlockSize);
                writeWord(preBlockEnd, addupBlockSize);
            }
            //第4种情况，前后均需要合并
            else
            {
                long nextBlockSize = getBlockLength(nextBlock);
                long long nextBlockStart = nextBlock - nextBlockSize - WORD_SIZE;

                long preBlockSize = getBlockLength(previousBlock);
                long long preBlockEnd = previousBlock + preBlockSize + WORD_SIZE;

                long addupBlockSize = size + nextBlockSize + preBlockSize + 4 * WORD_SIZE; //总共省下4个脚注空间！

                writeWord(nextBlockStart, addupBlockSize);
                writeWord(preBlockEnd, addupBlockSize);
            }
        }
    }
    else
    {
        cout << "尝试释放非堆区地址！" << endl;
        return false;
    }
}

long ProcessTableItem::getBlockLength(long long address)
{
    int page = address / mem_config.PAGE_SIZE;
    tableItem *ti = pageTable.at(page);
    if (!ti->isInMemory)
    {
        bool pageFaultRes = PageMemoryManager::getInstance()->pageFault(pid, ti);
        if (!pageFaultRes)
        {
            return false;
        }
    }
    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + address % mem_config.PAGE_SIZE;
    long res;
    memcpy(&res, (void *)realAddress, WORD_SIZE);
    PageMemoryManager::getInstance()->useFrame(fti);
    res ^= (-1 << 3); //最后三位不算；
    return res;
}

bool ProcessTableItem::isBlockFree(long long address)
{
    int page = address / mem_config.PAGE_SIZE;
    tableItem *ti = pageTable.at(page);
    if (!ti->isInMemory)
    {
        bool pageFaultRes = PageMemoryManager::getInstance()->pageFault(pid, ti);
        if (!pageFaultRes)
        {
            return false;
        }
    }
    FrameTableItem *fti = ti->frame;
    long long realAddress = fti->getFrameAddress() + address % mem_config.PAGE_SIZE;
    char res = *(char *)realAddress;
    PageMemoryManager::getInstance()->useFrame(fti);
    if (res ^ 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}