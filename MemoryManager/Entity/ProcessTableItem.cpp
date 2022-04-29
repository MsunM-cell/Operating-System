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
        codeLength += (WORD_SIZE - codeLength % WORD_SIZE); //����Ŷ
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
    long long heapBound = stackBase - 8 << 10; //����ջ������ַ����һ��
    heapBound -= (WORD_SIZE - 1);              //��˷�����Ҫ��С��ַ��ʼд���ݣ�
    /*
    ->  8 9 10 11  ---����ֹ      ��ͷ��ʾ��һ����Ӧ�ô�С��ַ��ʼ��
        4 5 6 7                  ���ȼ��㷽��Ϊ8-0-wordsize����Ϊ�����Ѿ�����һ����
    ->  0 1 2 3    ---����ʼ     ����Ϊ4����0.1.2.3����д4 ����8.9.10.11����Ҳд4��������һ���ʱ��0+4+2*Word Size
     */
    long freeSpace = heapBound - heapBase - WORD_SIZE;

    //��Ϊ���ȼ���������8�ı��������Ժ���λ���Գ�����á�����λΪ000���ʾδ���� �� 001��ʾ�ѷ���
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
        if (blockSize >= size) // blockSize������ͷβ8���ֽڵĳ���
        {
            if (isBlockFree(pointer))
            {
                if (size % WORD_SIZE != 0)
                {
                    size += (WORD_SIZE - size % WORD_SIZE); //���룬����ĳ���һ��Ϊ�ֵı���
                }

                long leftSpaceStart = pointer + size + 2 * WORD_SIZE; //�ָ�����һ���block��ʣ���һС����ֹ
                long leftSpaceEnd = pointer + blockSize + WORD_SIZE;
                if (leftSpaceEnd == leftSpaceStart || leftSpaceEnd - leftSpaceStart == WORD_SIZE)
                {
                    //����ָ�����һ���Ļ���ʣ�µ�һС��ռ��޷���ӽ�ע���ǾͰ�ʣ����һС��ռ�Ҳ�����
                    if (leftSpaceEnd == leftSpaceStart)
                        size += WORD_SIZE;
                    else if (leftSpaceEnd - leftSpaceStart == WORD_SIZE)
                    {
                        size += 2 * WORD_SIZE;
                    }
                }

                long spaceSize = size | 1;                        //�����һλΪ1˵����һ�鱻ռ��
                writeWord(pointer, spaceSize);                    //����
                writeWord(pointer + size + WORD_SIZE, spaceSize); //��β��

                if (size == blockSize)
                {
                    //ȫ�������������ü�����Ƭ����
                }
                else
                {
                    long leftSpaceStart1 = pointer + size + 2 * WORD_SIZE; //���¼�����Ƭ�����ֹ��ַ
                    long leftSpaceEnd1 = pointer + blockSize + WORD_SIZE;

                    long leftSpaceSize = leftSpaceEnd1 - leftSpaceStart1 - WORD_SIZE;
                    writeWord(leftSpaceStart1, leftSpaceSize);
                    writeWord(leftSpaceStart1, leftSpaceSize);
                }

                return pointer + WORD_SIZE; //��������
            }
        }
        pointer += blockSize + 2 * WORD_SIZE; //�����ڻ�ռ�þͿ���һ��
    } while (pointer < stackBase - 8 << 10);  //��ҪԽ��ջ����
    return -1;
}

bool ProcessTableItem::freeSpace(long long address)
{
    //���ȼ���ͷŵĵ�ַ�Ƿ��ڶ���
    long long heapBound = stackBase - 8 << 10 - WORD_SIZE + 1;
    if (address <= heapBound - WORD_SIZE || address >= heapBase + WORD_SIZE) //Ӧ����β������ע֮��
    {
        long long blockStart = address - WORD_SIZE; //�������ĵ�ַûʹ���Ļ�������Ӷ��м俪ʼ�ͷţ�����һ���־��ܵõ����׽�ע��
        if (isBlockFree(blockStart))
        {
            cout << "�����ͷ�δ����ռ䣡" << endl;
            return false;
        }
        long size = getBlockLength(blockStart);

        long blockEnd = blockStart + WORD_SIZE + size;

        //�ͷ�һ�οռ��Ӧ����4�������1.ǰ���Ϊռ���еĿ�  2.ǰ��Ϊ���п�   3.����Ϊ���п�  4.ǰ���Ϊ���п�
        long long previousBlock = blockStart + size + 2 * WORD_SIZE;
        long long nextBlock = blockStart - WORD_SIZE;

        if (previousBlock >= heapBound || !isBlockFree(previousBlock)) //û����һ�����һ�鱻ռ��
        {
            //��һ����������¾����úϲ�
            if (nextBlock < heapBase || !isBlockFree(nextBlock))
            {
                writeWord(blockStart, size);
                writeWord(blockEnd, size);
            }
            //��3��������������Ҫ�ϲ�
            else
            {
                long nextBlockSize = getBlockLength(nextBlock);
                long long nextBlockStart = nextBlock - nextBlockSize - WORD_SIZE;
                long addupBlockSize = nextBlockSize + size + 2 * WORD_SIZE; //ʡ�����м�������ע�Ŀռ�
                writeWord(nextBlockStart, addupBlockSize);
                writeWord(blockEnd, addupBlockSize);
            }
        }

        else //ǰ�����Ҫ�ϲ�
        {
            //û�к�����ռ�ã���2�������ֻ��ϲ�ǰһ��
            if (nextBlock < heapBase || !isBlockFree(nextBlock))
            {
                long preBlockSize = getBlockLength(previousBlock);
                long long preBlockEnd = previousBlock + preBlockSize + WORD_SIZE;
                long addupBlockSize = preBlockSize + size + 2 * WORD_SIZE;
                writeWord(blockStart, addupBlockSize);
                writeWord(preBlockEnd, addupBlockSize);
            }
            //��4�������ǰ�����Ҫ�ϲ�
            else
            {
                long nextBlockSize = getBlockLength(nextBlock);
                long long nextBlockStart = nextBlock - nextBlockSize - WORD_SIZE;

                long preBlockSize = getBlockLength(previousBlock);
                long long preBlockEnd = previousBlock + preBlockSize + WORD_SIZE;

                long addupBlockSize = size + nextBlockSize + preBlockSize + 4 * WORD_SIZE; //�ܹ�ʡ��4����ע�ռ䣡

                writeWord(nextBlockStart, addupBlockSize);
                writeWord(preBlockEnd, addupBlockSize);
            }
        }
    }
    else
    {
        cout << "�����ͷŷǶ�����ַ��" << endl;
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
    res ^= (-1 << 3); //�����λ���㣻
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