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

// ��
// ������
#define WIDTH 10
// ʱ��Ƭ��С
#define TIME_SLICE 200
// �����ȼ�
#define HIGH_PRI 0
// �����ȼ�
#define LOW_PRI 1
// ���ռ�õ�ʱ��Ƭ����
#define MAX_CNT 10


// CPU�Ƿ���ʹ�ã�����չΪ����
bool CPU[2];
// IO�豸�Ƿ���ʹ�ã�����չΪ����
bool IO[2];

// ȫ�ֱ���
// ����Ŀǰ��Ծ��PCB
static vector<PCB*> active_pcb;

static queue <PCB> FCFS;

// RR������
class RRQueue
{
private:
    //Ŀǰ������PCB������
    int size;
    //rr���б���
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

//���̹�������
class ProcManager
{
private:
public:

};

#endif // !PROC_H

