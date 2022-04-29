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
    //ÿ�����̵��߼��ռ��СΪ4MB
    static const long long LOGICAL_SPACE = 4 << 20;
    //�Ӹߵ�ַ���͵�ַ����Ϊ���ں���--������(����ҳ������ݽṹ)--ջ�� --��������������--����-------------������������������--������
    //��������Ϊ             1MB                                8kb    0kb           �ӻ���ַ��ջ��    0kb
    int pid;
    long long pageCount; //ʹ���е�ҳ��
    std::vector<tableItem *> pageTable;
    long long stackBase;
    long long rsp; //ָ�����ջ����Ҳ����˵��һ����������
    long long heapBase;
    long long brk; //�ѵĶ���
    long long dataStart;
    long long dataEnd;
    long long codeStart;
    long long codeEnd;

    void setFooter(long long start);

public:
    void insertPage();
    // TODO:�Ƿ������ջ��ջ����
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