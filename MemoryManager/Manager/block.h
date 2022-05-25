#pragma once
#include "mem.h"

#define head_addr first
#define len second

//容量递增排序方法
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//地址递增排序方法
bool cmp2(PII x, PII y) { return x.first < y.first; }

//动态分区分配管理
class BlockMemoryManager : public MemoryManager
{
private:
    //空闲分区链表
    vector<PII> free_block_table;
    //进程和首地址对应表
    map<int, PII> pid2addr;
    map<int, int> addr2pid;
    //进程指令长度
    map<int,int> ins_sum_len;
    //实例指针
    static BlockMemoryManager* instance;
    //内存监视
    static void monitor();
    //已使用内存大小
    int usedMem;
    //监控线程
    thread monitorThread;
    //是否正在工作
    static bool active;

public:
    //构造函数
    BlockMemoryManager();
    //析构函数
    ~BlockMemoryManager();
    //获取已使用内存大小
    int getUsedMem(){return usedMem;}
    //返回自己指针
    static BlockMemoryManager* getInstance();
    //空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
    void adjust_list(int type);
    //为进程分配内存
    int createProcess(PCB& p);
    //释放内存，传入参数pcb
    int freeProcess(PCB& p);
    //访问内存接口
    char accessMemory(int pid, int address);
    //写内存接口
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    //压缩
    int compress_mem();
    //加载指令
    int load_ins(int addr, int length,string path);
    //打印空闲分区链
    void print_list();

    string getMode(){return "block";}
    void dms_command();
};
