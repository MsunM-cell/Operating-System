/**
 * @file testRR.cpp
 * @brief �й���һ�����е�һЩ����
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/**
 * @brief RR���е�һ�㹹��
 * @param void
 */
RRQueue::RRQueue()
{
    this->size = 0;
}

/**
 * @brief �����ڲ�����ʹ�ã���������Ĵ�С�������
 * @param n_size �����в��ᱻ�����ĸ���
 * @param x_size �������ĸ���
 */
RRQueue::RRQueue(int n_size, int x_size)
{
    this->size = n_size + x_size;
    // ���ᱻ������PCB
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
    // ��������
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
 * @brief ��������
 */
RRQueue::~RRQueue()
{
    cout << "RR queue takes a nap.\n";
}

/**
 * @brief ��ѯĿǰ���еĳ���
 * @return ���г���
 */
int RRQueue::getSize()
{
    return this->size;
}

/**
 * @brief ��RR�����е��׸�PCB����
 * @return ָ�򵯳�PCB��ָ�룬������򷵻ؿ�ָ��
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
 * @brief ��ĳ��PCB�����β
 * @param target ָ����Ҫ����PCB��ָ��
 */
void RRQueue::pushBack(PCB* target)
{
    this->rr_que.push(target);
    this->size++;
}

/**
 * @brief ��������
 * @param target ��������PCB
 */
void RRQueue::downLevel(PCB* target)
{
    printf("[%ld]Pid %d down to fcfs.\n", clock() - system_start, target->id);
}

/**
 * @brief �������㷨
 * @return ������������0
 */
int RRQueue::scheduling()
{
    cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // ѭ����rr����Ϊ��
    while (this->getSize() != 0)
    {
        // ȡ����Ҫ�����PCB
        PCB* cur_pcb = this->popFront();
        if (cur_pcb != nullptr)
        {
            cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // �ж�ʱ���Ƿ����һ��ѭ��
            if (cur_pcb->time_need > TIME_SLICE) {
                // ģ��������
                Sleep(TIME_SLICE);
                // ʱ��Ƭ��
                cur_pcb->time_need -= TIME_SLICE;
                printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                       cur_pcb->time_need);
                // �ж�һ���Ƿ�ʹ���˹����ʱ��Ƭ,���򽵼�,��������β
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT) {
                    cur_pcb->pri = LOW_PRI;
                    // ��������fcfs������
                    this->downLevel(cur_pcb);
                } else {
                    this->pushBack(cur_pcb);
                }
            } else {
                // ��Ҫ��ʱ��С��������ʱ��Ƭ
                Sleep(cur_pcb->time_need);
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
            }
        }
    }
    return 0;
}

/**
 * @brief ProcManager�Ĺ��캯��
 */
ProcManager::ProcManager()
{
    cout << "ProcManager is running!\n";
    // �����ô���
    this->rr_queue = new RRQueue(5,2);
}

/**
 * @brief ProcManager����������
 */
ProcManager::~ProcManager()
{
    cout << "ProcManager is over!\n";
}

/**
 * ɱ��һ������
 * @param pid ���̵�id
 * @return �ɹ��򷵻�true
 */
bool ProcManager::kill(int pid)
{
    for (auto it=this->active_pcb.begin(); it < this->active_pcb.end(); it++)
    {
        // �ҵ�Ŀ����̲�����
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
    // ϵͳ��ʼ��ʱ��ʵ��Ӧ�ø���
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