#pragma once
#include "mem.h"

//页表寄存器
#define start_addr first
#define table_len second


//基本分页管理
class BasicPageManager : public MemoryManager
{
private:
    //页表寄存器,随进程切换而切换
    PII PTR;
    //所有进程快表模拟,随PTR切换而切换
    unordered_map<int, vector<PII>> TLB;
    //进程指令长度
    map<int,int> ins_sum_len;
    //初始化基本分页系统
    void init_manager();
    //标记帧使用情况,first为进程pid,初始为-1,second为该帧实际使用大小
    vector<PII> bitmap;
    //(所有)进程页表
    map<int,vector<int>> pagetable;

public:
    //构造函数和析构函数
    BasicPageManager() { init_manager(); }
    ~BasicPageManager(){}
    //访问内存接口
    char accessMemory(int pid, int address);
    //写内存接口
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    //加载指令
    int load_ins(int pid, string Path);
    //分配进程内存
    int createProcess(PCB& p);
    //释放内存
    int freeProcess(PCB& p);
    //获取当前进程页表地址
    void set_PTR(const PCB& p)
    {
        PTR.start_addr = p.pagetable_addr;
        PTR.table_len = p.pagetable_len;
    }
    //打印帧使用情况
    void print_frame();
    //打印进程页表
    void print_pagetable(const PCB& p);
    
    string getMode(){return "page";}
    void dms_command();
};
