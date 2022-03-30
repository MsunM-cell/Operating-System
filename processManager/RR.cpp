/**
 * @file testRR.cpp
 * @brief �й���һ�����е�һЩ����
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/**
 * @brief �����ڲ�����ʹ�ã���������Ĵ�С�������
 * @param n_size �����в��ᱻ�����ĸ���
 * @param x_size �������ĸ���
 */
RRQueue::RRQueue(int n_size, int x_size)
{
    // ���ᱻ������PCB
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
    // ��������
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
    return (int)(this->rr_que.size());
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
 * �Ƴ�һ��pcb
 * @param pid ��Ҫ�Ƴ���pcb��id
 * @return �ɹ��򷵻�true
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
 * @brief �������㷨
 * @return ������������0
 */
int RRQueue::scheduling()
{
    cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // ѭ����rr����Ϊ��
    while (!this->rr_que.empty())
    {
        // ��ÿ��pcb���д���
        for (auto it = this->rr_que.begin(); it < this->rr_que.end(); it++)
        {
            PCB* cur_pcb = *it;
            cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // �ж�ʱ���Ƿ����һ��ѭ��
            if (cur_pcb->time_need > TIME_SLICE)
            {
                // ģ��������
                Sleep(TIME_SLICE);
                // ʱ��Ƭ��
                cur_pcb->time_need -= TIME_SLICE;
                printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                       cur_pcb->time_need);
                // �ж�һ���Ƿ�ʹ���˹����ʱ��Ƭ,���򽵼�
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT)
                {
                    cur_pcb->pri = LOW_PRI;
                    // ��������fcfs������,���ӵ�ǰ����ɾ��
                    this->downLevel(cur_pcb);
                    it = this->rr_que.erase(it);
                }
            }
            else
            {
                // ��Ҫ��ʱ��С��������ʱ��Ƭ����ɺ�Ӷ�����ɾ������
                Sleep(cur_pcb->time_need);
                cur_pcb->time_need = -1;
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
                it = this->rr_que.erase(it);
                //TODO ���ʵ�ֵ��ⲿ�Ķ��и��£�
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
    // �ڻ�Ծ��������Ѱ�ҽ���
    for (auto it=this->active_pcb.begin(); it < this->active_pcb.end(); it++)
    {
        // �ҵ��ˣ�ɾ����������Ӷ�Ӧ�ĵ��ȶ�����ɾ��
        if ((*it)->id == pid)
        {
            if ((*it)->pri == HIGH_PRI)
            {
                this->rr_queue->removePCB(pid);
            }
            // else if ((*it)->pri == LOW_PRI)
            // {
            // //    ��fcfs����������
            // }
            delete *it;
            this->active_pcb.erase(it);
            return true;
        }
    }
    // �ڵȴ���������Ѱ�ҽ���
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // �ҵ��ˣ�ɾ����
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
 * �г���ǰ���д��ڵ�pcb
 */
void ProcManager::ps()
{
    //TODO ����дps
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