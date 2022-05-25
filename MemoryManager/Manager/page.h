/*
@Author:Yuxing Long
@Date:2022.5.25
*/

#pragma once
#include "mem.h"
#include <thread>

//页表寄存器
#define start_addr first
#define table_len second

//基本分页管理
class BasicPageManager : public MemoryManager
{
private:
    //进程指令长度
    map<int, int> ins_sum_len;
    //标记帧使用情况,first为进程pid,初始为-1,second为该帧实际使用大小
    vector<PII> bitmap;
    //(所有)进程页表
    map<int, vector<int>> pagetable;
    //监控线程
    thread monitorThread;
    //监控内存
    static void monitor();
    //自己实例
    static BasicPageManager *instance;
    //已使用的页数
    int usedPage;
    //是否正在工作
    static bool active;

public:
    //构造函数和析构函数
    BasicPageManager();
    ~BasicPageManager();
    int getUsedPage() { return usedPage; }
    //获取实例
    static BasicPageManager *getInstance();
    //访问内存接口
    char accessMemory(int pid, int address);
    //写内存接口
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    //加载指令
    int load_ins(int pid, string Path);
    //分配进程内存
    int createProcess(PCB &p);
    //释放内存
    int freeProcess(PCB &p);
    //打印帧使用情况
    void print_frame();
    //打印进程页表
    void print_pagetable(const PCB &p);

    string getMode() { return "page"; }
    void dms_command();
};
