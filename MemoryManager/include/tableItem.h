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
    long long pageNo;      // TODO:��û�б�Ҫ��
    bool isInMemory;       //�Ƿ����ڴ���
    bool isChange;         //�޸�λ
    long long swapAddress; //����-1���ʾ���������
};

#endif