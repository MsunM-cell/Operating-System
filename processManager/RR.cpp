/**
 * @file testRR.cpp
 * @brief 有关于一级队列的一些尝试
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/**
 * @brief 仅会在测试中使用，根据输入的大小随机构造
 * @param n_size 队列中不会被降级的个数
 * @param x_size 被降级的个数
 */
RRQueue::RRQueue(int n_size, int x_size)
{
    // 不会被降级的PCB
    for (int i = 0; i < n_size; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = i;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 30 * 100;
        active_pcb.push_back(new_pcb);
        this->rr_que.push_back(new_pcb);
    }
    // 降级测试
    for (int i = 0; i < x_size; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = n_size + i;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 10 * 100 + TIME_SLICE * MAX_CNT;
        active_pcb.push_back(new_pcb);
        this->rr_que.push_back(new_pcb);
    }
}

/**
 * @brief 析构函数
 */
RRQueue::~RRQueue()
{
    cout << "RR queue takes a nap.\n";
}

/**
 * @brief 查询目前队列的长度
 * @return 队列长度
 */
int RRQueue::getSize()
{
    return (int)(this->rr_que.size());
}

/**
 * @brief 降级操作
 * @param target 被降级的PCB
 */
void RRQueue::downLevel(PCB* target)
{
    printf("[%ld]Pid %d down to fcfs.\n", clock() - system_start, target->id);
}

/**
 * 移除一个pcb
 * @param pid 将要移除的pcb的id
 * @return 成功则返回true
 */
bool RRQueue::removePCB(int pid)
{
    for (auto it = this->rr_que.begin(); it != this->rr_que.end(); it++)
    {
        if ((*it)->id == pid)
        {
            this->rr_que.erase(it);
            return true;
        }
    }
    return false;
}

/**
 * @brief 调度主算法
 * @return 正常结束返回0
 */
int RRQueue::scheduling()
{
    cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // 循环到rr队列为空
    while (!this->rr_que.empty())
    {
        // 对每个pcb进行处理
        for (auto it = this->rr_que.begin(); it < this->rr_que.end(); it++)
        {
            PCB* cur_pcb = *it;
            cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // 判断时间是否够完成一次循环
            if (cur_pcb->time_need > TIME_SLICE)
            {
                // 模拟服务过程
                Sleep(TIME_SLICE);
                // 时间片到
                cur_pcb->time_need -= TIME_SLICE;
                printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                       cur_pcb->time_need);
                // 判断一下是否使用了过多的时间片,是则降级
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT)
                {
                    cur_pcb->pri = LOW_PRI;
                    // 降级加入fcfs队列中,并从当前队列删除
                    this->downLevel(cur_pcb);
                    it = this->rr_que.erase(it);
                }
            }
            else
            {
                // 需要的时间小于完整的时间片，完成后从队列中删除该项
                Sleep(cur_pcb->time_need);
                cur_pcb->time_need = -1;
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
                it = this->rr_que.erase(it);
                //TODO 如何实现到外部的队列更新？
            }
        }
    }
    return 0;
}

/**
 * @brief ProcManager的构造函数
 */
ProcManager::ProcManager()
{
    cout << "ProcManager is running!\n";
    // 测试用代码
    this->rr_queue = new RRQueue(5,2);
}

/**
 * @brief ProcManager的析构函数
 */
ProcManager::~ProcManager()
{
    cout << "ProcManager is over!\n";
}

/**
 * 杀掉一个进程
 * @param pid 进程的id
 * @return 成功则返回true
 */
bool ProcManager::kill(int pid)
{
    // 在活跃队列里面寻找进程
    for (auto it=this->active_pcb.begin(); it < this->active_pcb.end(); it++)
    {
        // 找到了，删除它并将其从对应的调度队列中删除
        if ((*it)->id == pid)
        {
            if ((*it)->pri == HIGH_PRI)
            {
                this->rr_queue->removePCB(pid);
            }
            // else if ((*it)->pri == LOW_PRI)
            // {
            // //    到fcfs队列里面找
            // }
            delete *it;
            this->active_pcb.erase(it);
            return true;
        }
    }
    // 在等待队列里面寻找进程
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // 找到了，删除它
        if ((*it)->id == pid)
        {
            delete *it;
            this->active_pcb.erase(it);
            return true;
        }
    }
    return false;
}

/**
 * 列出当前所有存在的pcb
 */
void ProcManager::ps()
{
    //TODO 继续写ps
}

int main()
{
    RRQueue test_queue(5,2);
    // 系统开始计时，实际应该更早
    system_start = clock();
    cout << setiosflags(ios::left);
    printf("[%ld]This is a RR test\n", clock() - system_start);

    test_queue.scheduling();

    for (PCB* cur : active_pcb)
    {
        delete cur;
    }
    return 0;
}