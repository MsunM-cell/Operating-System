/**
 * @file testRR.cpp
 * @brief 有关于一级队列的一些尝试
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/**
 * @brief RR队列的一般构造
 * @param void
 */
RRQueue::RRQueue()
{
    this->size = 0;
}

/**
 * @brief 仅会在测试中使用，根据输入的大小随机构造
 * @param n_size 队列中不会被降级的个数
 * @param x_size 被降级的个数
 */
RRQueue::RRQueue(int n_size, int x_size)
{
    this->size = n_size + x_size;
    // 不会被降级的PCB
    for (int i = 0; i < n_size; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = i;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 30 * 100;
        active_pcb.push_back(new_pcb);
        this->rr_que.push(new_pcb);
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
        this->rr_que.push(new_pcb);
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
    return this->size;
}

/**
 * @brief 将RR队列中的首个PCB弹出
 * @return 指向弹出PCB的指针，如果空则返回空指针
 */
PCB* RRQueue::popFront()
{
    if (this->rr_que.empty())
    {
        return nullptr;
    }
    PCB* cur_pcb =  this->rr_que.front();
    this->rr_que.pop();
    this->size--;
    return cur_pcb;
}

/**
 * @brief 将某个PCB加入队尾
 * @param target 指向需要插入PCB的指针
 */
void RRQueue::pushBack(PCB* target)
{
    this->rr_que.push(target);
    this->size++;
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
 * @brief 调度主算法
 * @return 正常结束返回0
 */
int RRQueue::scheduling()
{
    cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // 循环到rr队列为空
    while (this->getSize() != 0)
    {
        // 取出需要处理的PCB
        PCB* cur_pcb = this->popFront();
        if (cur_pcb != nullptr)
        {
            cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // 判断时间是否够完成一次循环
            if (cur_pcb->time_need > TIME_SLICE) {
                // 模拟服务过程
                Sleep(TIME_SLICE);
                // 时间片到
                cur_pcb->time_need -= TIME_SLICE;
                printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                       cur_pcb->time_need);
                // 判断一下是否使用了过多的时间片,是则降级,否则加入队尾
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT) {
                    cur_pcb->pri = LOW_PRI;
                    // 降级加入fcfs队列中
                    this->downLevel(cur_pcb);
                } else {
                    this->pushBack(cur_pcb);
                }
            } else {
                // 需要的时间小于完整的时间片
                Sleep(cur_pcb->time_need);
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
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
    for (auto it=this->active_pcb.begin(); it < this->active_pcb.end(); it++)
    {
        // 找到目标进程并消灭
        if ((*it)->id == pid)
        {
            delete *it;
            this->active_pcb.erase(it);
        }
    }
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