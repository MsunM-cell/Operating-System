#ifndef PROC_H
#define PROC_H

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <queue>
#include <vector>
#include <cmath>
#include <windows.h>
using namespace std;

// 宏
// 对齐宽度
#define WIDETH 10
// 时间片大小
#define TIME_SLICE 200
// 高优先级
#define HIGH_PRI 0
// 低优先级
#define LOW_PRI 1
// 最大时间片数量
#define MAX_CNT 10

// PCB的结构体
typedef struct 
{
   // pid
   int id;
   // 优先级
   int pri; 
   // 预计还需要的时间
   int time_need;
} PCB;

// CPU是否在使用，可扩展为数组
bool CPU[2];
// IO设备是否在使用，可扩展为数组
bool IO[2];

// 全局变量
// rr队列
static queue<PCB*> rr_que;
// 保存目前活跃的PCB
static vector<PCB*> active_pcb;
// 程序运行的开始时间
clock_t system_start;
// 时间片开始的时间
static clock_t slice_start;

static queue <PCB> FCFS;

#endif // !PROC_H

