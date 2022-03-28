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

// ��
// ������
#define WIDETH 10
// ʱ��Ƭ��С
#define TIME_SLICE 200
// �����ȼ�
#define HIGH_PRI 0
// �����ȼ�
#define LOW_PRI 1
// ���ʱ��Ƭ����
#define MAX_CNT 10

// PCB�Ľṹ��
typedef struct 
{
   // pid
   int id;
   // ���ȼ�
   int pri; 
   // Ԥ�ƻ���Ҫ��ʱ��
   int time_need;
} PCB;

// CPU�Ƿ���ʹ�ã�����չΪ����
bool CPU[2];
// IO�豸�Ƿ���ʹ�ã�����չΪ����
bool IO[2];

// ȫ�ֱ���
// rr����
static queue<PCB*> rr_que;
// ����Ŀǰ��Ծ��PCB
static vector<PCB*> active_pcb;
// �������еĿ�ʼʱ��
clock_t system_start;
// ʱ��Ƭ��ʼ��ʱ��
static clock_t slice_start;

static queue <PCB> FCFS;

#endif // !PROC_H

