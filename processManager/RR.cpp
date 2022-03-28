/**
 * @file testRR.cpp
 * @brief �й���һ�����е�һЩ����
 * @date 2022-03-25
 */
#include <iostream>
#include "proc.h"
using namespace std;

extern queue<PCB*> rr_que;
extern vector<PCB*> active_pcb;
extern clock_t system_start, slice_start;

void init()
{
    srand(time(NULL));
    for (int i = 0; i < 5; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = i;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 30 * 100;
        active_pcb.push_back(new_pcb);
        rr_que.push(new_pcb);
    }
    // ��������
    PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
    new_pcb->id = 5;
    new_pcb->pri = HIGH_PRI;
    new_pcb->slice_cnt = 0;
    new_pcb->time_need = 5000;
    active_pcb.push_back(new_pcb);
    rr_que.push(new_pcb);

}

int main(void)
{
    init();
    // ϵͳ��ʼ��ʱ��ʵ��Ӧ�ø���
    system_start = clock();
    printf("[%ld]This is a RR test\n", clock() - system_start);
    cout << setiosflags(ios::left);
    cout << setw(WIDETH) << "Id" << setw(WIDETH) << "Time_need\n";

    // ѭ����rr����Ϊ��
    while (rr_que.empty() != true)
    {
        PCB* cur_pcb =  rr_que.front();
        rr_que.pop();
        cout << setw(WIDETH) << cur_pcb->id << setw(WIDETH) << cur_pcb->time_need << endl;
        slice_start = clock();
        // �ж�ʱ���Ƿ����һ��ѭ��
        if (cur_pcb->time_need > TIME_SLICE)
        {
            Sleep(TIME_SLICE);
            cur_pcb->time_need -= TIME_SLICE;
            printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id, cur_pcb->time_need);
            // �ж�һ���Ƿ�ʹ���˹����ʱ��Ƭ,���򽵼�
            cur_pcb->slice_cnt++;
            if (cur_pcb->slice_cnt == 10)
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