/*
 * @Date: 2022-04-01 16:54:08
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-23 12:37:08
 * @FilePath: \Operating-System\MemoryManager\include\FrameTableItem.h
 */

#ifndef FRAMMETABLEITEM
#define FRAMMETABLEITEM
#include"tableItem.h"
class FrameTableItem
{
private:
    long long frameAddress; //该帧的真实物理地址，可以通过Memory[frameAddress]直接访问
    tableItem *logicalPage;
    unsigned int frameNo;
    unsigned int pid;
    // TODO:是否删除occupied
    bool occupied; //是否被某个进程占用 ， 或者说直接使用pid来判断
    bool locked;

public:
    FrameTableItem *next, *pre;
    FrameTableItem(long long frameAddress,unsigned int frameNo);
    ~FrameTableItem(){};
    void setFrameAddress(long long frameAddress) { this->frameAddress = frameAddress; }
    long long getFrameAddress() { return frameAddress; }
    int getPid(){return pid;}
    void setPid(unsigned int pid) { this->pid = pid; }
    void setLogicalPage(tableItem *ti) { this->logicalPage = ti; }
    /**
     * @brief 
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
    unsigned int getFrameNo(){return frameNo;}
    void setOccupied() { occupied = true; }
    bool isLocked() { return locked; }
    void setLock() { locked = true; }
    void unlock() { locked = false; }
};

#endif