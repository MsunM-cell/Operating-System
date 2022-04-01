#ifndef PROC_H
#define PROC_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <queue>
#include <vector>
#include <cmath>
#include <windows.h>
#include <string>

#include "../MemoryManager/Manager/PageMemoryManager.cpp"
#include "../lib/sys.h"
using namespace std;

// 宏
// 对齐宽度
#define WIDTH 10
// 时间片大小
#define TIME_SLICE 200
// 高优先级
#define HIGH_PRI 0
// 低优先级
#define LOW_PRI 1
// 最大占用的时间片数量
#define MAX_CNT 10
// 最大可调度的进程数量
#define MAX_PROC 10


// CPU是否在使用，可扩展为数组
bool CPU[2];
// IO设备是否在使用，可扩展为数组
bool IO[2];

// 全局变量
// 保存目前活跃的PCB
static vector<PCB*> active_pcb;

static queue <PCB> FCFS;

// RR队列类
class RRQueue
{
private:
    //rr队列本身
    vector<PCB*> rr_que;
public:
    RRQueue() = default;
    ~RRQueue();
    int getSize();
    bool addPCB(PCB* target);
    bool removePCB(int pid);
    void downLevel(PCB* target);
    int scheduling();
    void getInfo();
    void getInfo(int pid);
};


class ProcManagerFCFS{
public:
    void addToQueue(PCB p);
    void runProcManager(PageMemoryManager* m);
    bool removeProc(int pid);
    ~ProcManagerFCFS();
    void getFcfsInfo();
    void getFcfsInfo(int pid);
    int getQueueSize();
private:
    vector <PCB> fcfsQueue;
    void run(PCB p,PageMemoryManager* m);
    string getCommand();

};

//进程管理器类
class ProcManager
{
private:
    // 记录可用的pid号
    int cpid;
    // 处于就绪状态的pcb，这个东西应该不存在，假装它来自内存
    vector<PCB*> active_pcb;
    // 位于等待状态的pcb
    vector<PCB*> waiting_pcb;
    // RR队列
    RRQueue* rr_queue;
    ProcManagerFCFS* fsfcProcManager;
public:
    ProcManager();
    ProcManager(int n_size, int x_size);
    ~ProcManager();
    int getActiveNum();
    void kill();
    void kill(int pid);
    void run(string file_name);
    void ps();
    void ps(int pid);
    void scheduling();
};

#endif // !PROC_H

