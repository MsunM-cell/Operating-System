#pragma once
#include "mem.h"

#define head_addr first
#define len second

//容量递增排序方法
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//地址递增排序方法
bool cmp2(PII x, PII y) { return x.first < y.first; }

//动态分区分配管�?
class BlockMemoryManager : public MemoryManager
{
private:
    //空闲分区链表
    vector<PII> free_block_table;
    //进程和首地址对应�?
    map<int, PII> pid2addr;
    map<int, int> addr2pid;
    //进程指令长度
    map<int,int> ins_sum_len;
    //读取配置文件，修改分配策�?
    void modify_tactic(int new_mode) { mem_config.BLOCK_ALGORITHM = new_mode; }
    //初始化该内存管理系统
    void init_manager();

public:
    //构造函�?
    BlockMemoryManager() { init_manager(); }
    //析构函数
    ~BlockMemoryManager() { printf("Exit the dynamic block system\n"); }
    //空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
    void adjust_list(int type);
    //搜索满足条件的空闲块，并返回空闲块首地址
    int createProcess(PCB& p);
    //释放内存，传入参数pcb
    int freeProcess(PCB& p);
    //访问内存接口
    char accessMemory(int pid, int address);
    //写内存接�?
    int writeMemory(int logicalAddress, char src, unsigned int pid);
    //压缩
    int compress_mem();
    //加载指令
    int load_ins(int addr, int length,string path);
    //打印空闲分区�?
    void print_list();
};
