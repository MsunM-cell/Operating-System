/*
 * @Date: 2022-04-01 16:54:08
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 13:52:05
 * @FilePath: \Operating-System\MemoryManager\include\FrameTableItem.h
 */

#ifndef FRAMMETABLEITEM
#define FRAMMETABLEITEM
#include"tableItem.h"
class FrameTableItem
{
private:
    long long frameAddress; //��֡����ʵ�����ַ������ͨ��Memory[frameAddress]ֱ�ӷ���
    tableItem *logicalPage;
    unsigned int frameNo;
    unsigned int pid;
    // TODO:�Ƿ�ɾ��occupied
    bool occupied; //�Ƿ�ĳ������ռ�� �� ����˵ֱ��ʹ��pid���ж�
    bool locked;

public:
    FrameTableItem *next, *pre;
    FrameTableItem(long long frameAddress,unsigned int frameNo);
    ~FrameTableItem();
    void setFrameAddress(long long frameAddress) { this->frameAddress = frameAddress; }
    long long getFrameAddress() { return frameAddress; }
    void setPid(unsigned int pid) { this->pid = pid; }
    void setLogicalPage(tableItem *ti) { this->logicalPage = ti; }
    /**
     * @description: 
     * @return {��֡��Ӧ��ҳ}
     */    
    tableItem *getLogicalPage() { return logicalPage; }
    bool isOccupied() { return occupied; }
    void unUsed()
    {
        occupied = false;
        pid = -1;
        logicalPage = nullptr;
        locked = false;
    }
    unsigned int getFrameNo(){return frameNo;}
    void setOccupied() { occupied = true; }
    bool isLocked() { return locked; }
    void setLock() { locked = true; }
    void unlock() { locked = false; }
};

#endif