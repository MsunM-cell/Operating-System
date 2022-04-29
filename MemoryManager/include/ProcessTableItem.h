/*
 * @Date: 2022-04-01 15:07:54
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-29 10:52:01
 * @FilePath: \Operating-System\MemoryManager\include\ProcessTableItem.h
 */
#ifndef PROCESSTABLEITEM
#define PROCESSTABLEITEM

#include <vector>
#include "tableItem.h"
#include "../Manager/mem.h"
#include "../include/PageMemoryManager.h"

class ProcessTableItem
{
private:
    static const int WORD_SIZE = 4;
    //每个进程的逻辑空间大小为4MB
    static const long long LOGICAL_SPACE = 4 << 20;
    //从高地址到低地址依次为：内核区--进程区(储存页表等数据结构)--栈区 --【共享数据区】--堆区-------------进程数据区（变量）--代码区
    //长度依次为             1MB                                8kb    0kb           从基地址到栈顶    0kb
    int pid;
    long long pageCount; //使用中的页数
    std::vector<tableItem *> pageTable;
    long long stackBase;
    long long rsp; //指向的是栈顶，也就是说下一个才是数据
    long long heapBase;
    long long brk; //堆的顶部
    long long dataStart;
    long long dataEnd;
    long long codeStart;
    long long codeEnd;

    void setFooter(long long start);

public:
    void insertPage();
    // TODO:是否添加入栈出栈操作
    ProcessTableItem(int pid, long long size, long long codeLength);
    void initHeap();
    bool writeWord(long long address, long word);
    bool writeChar(long long address, char c);
    long long allocate(long length);
    long getBlockLength(long long address);
    bool isBlockFree(long long address);
    bool freeSpace(long long address);
};

#endif