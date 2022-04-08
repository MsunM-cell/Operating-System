/*
 * @Date: 2022-04-01 16:54:08
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:20:12
 * @FilePath: \Operating-System\MemoryManager\include\FrameTableItem.h
 */

#include"tableItem.h"

class FrameTableItem
{
private:
    unsigned long frameAddress; //��֡����ʵ�����ַ������ͨ��Memory[frameAddress]ֱ�ӷ���
    tableItem *logicalPage;
    unsigned int pid;
    // TODO:�Ƿ�ɾ��occupied
    bool occupied; //�Ƿ�ĳ������ռ�� �� ����˵ֱ��ʹ��pid���ж�
    bool locked;

public:
    FrameTableItem *next, *pre;
    FrameTableItem(unsigned long frameAddress);
    ~FrameTableItem();
    void setFrameAddress(unsigned long frameAddress) { this->frameAddress = frameAddress; }
    unsigned long getFrameAddress() { return frameAddress; }
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
    void setOccupied() { occupied = true; }
    bool isLocked() { return locked; }
    void setLock() { locked = true; }
    void unlock() { locked = false; }
};