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
#include <map>

#include "../lib/sys.h"
using namespace std;

// 宏
// 对齐宽度
#define WIDTH 10
// 时间片大小
#define TIME_SLICE 100
// 一个因子，用于放大模拟时间
#define ALPHA 10
// 高优先级
#define HIGH_PRI 0
// 低优先级
#define LOW_PRI 1
// 最大占用的时间片数量
#define MAX_CNT 10
// 最大可调度的进程数量
#define MAX_PROC 5


// CPU是否在使用，可扩展为数组
bool CPU = true;
// IO设备是否在使用，可扩展为数组
bool IO = true;
// 屏幕输出是否在使用
bool screen = true;
// 键盘是否在使用
bool keyboard = true;

// 全局变量
// 保存目前活跃的PCB
// static vector<PCB*> active_pcb;

static queue <PCB> FCFS;


class ProcManagerFCFS{
public:
    void addToQueue(PCB *p);
    void runProcManager();
    bool removeProc(int pid);
    ProcManagerFCFS();
    ~ProcManagerFCFS();
    void getFcfsInfo();
    PCB* getFcfsInfo(int pid);
    int getQueueSize();
private:
    vector <PCB*> fcfsQueue;
    map<string,int> commandMap;
    void run(PCB *p);
    string getCommand(PCB *p);
    void initCmdMap();
    void useCPU(string command);
    void useIO(string command);
    void accessMem(string command,int pid);
    void writeMem(string command,int pid);
    string splitCommand(string command);
};

// RR队列类
class RRQueue
{
private:
    //rr队列本身
    vector<PCB*> rr_que;
    map<string,int> commandMap;
    string getCommand(PCB *p);
    void initCmdMap();
    void useCPU(string command);
    void useIO(string command);
    void accessMem(string command);
    void writeMem(string command);
    string splitCommand(string command);
public:
    RRQueue();
    ~RRQueue();
    int getSize();
    bool addPCB(PCB* target);
    bool removePCB(int pid);
    void downLevel(PCB* target,ProcManagerFCFS* fcfs);
    int scheduling(ProcManagerFCFS* fcfs);
    void getInfo();
    void exec(PCB *p,int &time);
    PCB* getInfo(int pid);
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
    // 阻塞队列
    vector<vector<PCB*>> block_pcb;
    // RR队列
    RRQueue* rr_queue;
    ProcManagerFCFS* fcfsProcManager;
    ProcManager();
    ProcManager(int n_size, int x_size);
    ~ProcManager();
public:
    int getActiveNum();
    void kill(int pid);
    void run(string file_name);
    void run(string file_name, int time);
    void run(PCB* pcb);
    void ps();
    void ps(int pid);
    void scheduling();
    bool freePCB(PCB* target);
    void maintain(int time_pass);
    int getAvailableId();
    void block(PCB* pcb, int dev);
    static ProcManager& getInstance();
};

#endif // !PROC_H

