/**
 * @file testRR.cpp
 * @brief �й���һ�����е�һЩ����
 * @date 2022-03-25
 */
#include <iostream>
#include "proc.h"
using namespace std;


RRQueue::RRQueue()
{
    this->size = 0;
}

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

RRQueue::~RRQueue()
{
    cout << "RR queue takes a nap.\n";
}

int RRQueue::getSize()
{
    return this->size;
}

int main()
{
    RRQueue test_queue(5,2);
    // ϵͳ��ʼ��ʱ��ʵ��Ӧ�ø���
    system_start = clock();
    printf("[%ld]This is a RR test\n", clock() - system_start);
    cout << setiosflags(ios::left);
    cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";

    // ѭ����rr����Ϊ��
    while (test_queue.getSize() != true)
    {
        PCB* cur_pcb =  rr_que.front();
        rr_que.pop();
        cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
        // �ж�ʱ���Ƿ����һ��ѭ��
        if (cur_pcb->time_need > TIME_SLICE)
        {
            Sleep(TIME_SLICE);
            cur_pcb->time_need -= TIME_SLICE;
            printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id, cur_pcb->time_need);
            // �ж�һ���Ƿ�ʹ���˹����ʱ��Ƭ,���򽵼�
            cur_pcb->slice_cnt++;
            if (cur_pcb->slice_cnt == MAX_CNT)
            {
                cur_pcb->pri = LOW_PRI;
                // ��������fcfs������
                printf("[%ld]Pid %d down to fcfs.\n", clock() - system_start, cur_pcb->id);
            }
            else {
                rr_que.push(cur_pcb);
            }
        }
        else
        {
            Sleep(cur_pcb->time_need);
            printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
        }

    }

    for (PCB* cur : active_pcb)
    {
        delete cur;
    }
    return 0;
}