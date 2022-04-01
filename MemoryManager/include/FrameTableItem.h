/*
 * @Date: 2022-04-01 16:54:08
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 19:24:30
 * @FilePath: \Operating-System\MemoryManager\include\FrameTableItem.h
 */
class FrameTableItem
{
private:
    unsigned long frameAddress;
    unsigned long logicalPage;
    unsigned int pid;
    bool occupied;
public:
    FrameTableItem(unsigned long frameAddress);
    ~FrameTableItem();
    void setFrameAddress(unsigned long frameAddress){this->frameAddress = frameAddress;}
    unsigned long getFrameAddress(){return frameAddress;}
    void setPid(unsigned int pid){this->pid = pid;}
    void setLogicalPage(unsigned long logicalPage){this->logicalPage = logicalPage;}
    bool isOccupied(){return occupied;}
    
};