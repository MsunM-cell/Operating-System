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
    //rr���б���
    vector<PCB*> rr_que;
public:
    RRQueue() = default;
    RRQueue(int n_size, int x_size);
    ~RRQueue();
    int getSize();
    bool removePCB(int pid);
    void downLevel(PCB* target);
    int scheduling();
};

//���̹�������
class ProcManager
{
private:
    // ���ھ���״̬��pcb
    vector<PCB*> active_pcb;
    // λ�ڵȴ�״̬��pcb
    vector<PCB*> waiting_pcb;
    // RR����
    RRQueue* rr_queue;
    procManagerFCFS* fsfcProcManager;
public:
    ProcManager();
    ~ProcManager();
    bool kill(int pid);
    bool run(XFILE file);
    void ps();
    void ps(int pid);
};

class procManagerFCFS{
    public:
        void addToQueue(PCB p);
        void runProcManager(PageMemoryManager* m);
        bool removeProc(int pid);
        ~procManagerFCFS();
    private:
        vector <PCB> fcfsQueue;
        void run(PCB p,PageMemoryManager* m);
        string getCommand();
    
};


#endif // !PROC_H

