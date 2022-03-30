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
    //目前队列中PCB的数量
    int size;
    //rr队列本身
    queue<PCB*> rr_que;
public:
    RRQueue();
    RRQueue(int n_size, int x_size);
    ~RRQueue();
    int getSize();
    PCB* popFront();
    void pushBack(PCB* target);
    void downLevel(PCB* target);
    int scheduling();
};

//进程管理器类
class ProcManager
{
private:
public:

};

#endif // !PROC_H

