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
    unsigned long frameAddress; //该帧的真实物理地址，可以通过Memory[frameAddress]直接访问
    tableItem *logicalPage;
    unsigned int pid;
    // TODO:是否删除occupied
    bool occupied; //是否被某个进程占用 ， 或者说直接使用pid来判断
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
     * @return {该帧对应的页}
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