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
#define WIDTH 10
// 时间片大小
#define TIME_SLICE 200
// 高优先级
#define HIGH_PRI 0
// 低优先级
#define LOW_PRI 1
// 最大占用的时间片数量
#define MAX_CNT 10

// PCB的结构体
typedef struct 
{
   // pid
   int id;
   // 优先级
   int pri;
   // 使用过的时间片数量
   int slice_cnt;
   // 预计还需要的时间
   int time_need;
} PCB;

// CPU是否在使用，可扩展为数组
bool CPU[2];
// IO设备是否在使用，可扩展为数组
bool IO[2];

// 全局变量
// rr队列
// static queue<PCB*> rr_que;
// 保存目前活跃的PCB
static vector<PCB*> active_pcb;
// 程序运行的开始时间
clock_t system_start;

static queue <PCB> FCFS;

/**
 * @brief RR队列类
 */
class RRQueue {
private:
    //目前队列中PCB的数量
    int size;
    //rr队列本身
    queue<PCB*> rr_que;
public:
    /**
     * @brief RR队列的一般构造
     * @param void
     */
    RRQueue();
    /**
     * @brief 仅会在测试中使用，根据输入的大小随机构造
     * @param n_size 队列中不会被降级的个数
     * @param x_size 被降级的个数
     */
    RRQueue(int n_size, int x_size);
    /**
     * @brief 析构函数
     */
    ~RRQueue();
    /**
     * @brief 查询目前队列的长度
     * @return 队列长度
     */
    int getSize();
};

#endif // !PROC_H

