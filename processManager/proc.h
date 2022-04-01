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
#include <string>

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
// ���ɵ��ȵĽ�������
#define MAX_PROC 10


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
    ~RRQueue();
    int getSize();
    bool addPCB(PCB* target);
    bool removePCB(int pid);
    void downLevel(PCB* target);
    int scheduling();
    void getInfo();
    void getInfo(int pid);
};


class ProcManagerFCFS{
public:
    void addToQueue(PCB p);
    void runProcManager();
    bool removeProc(int pid);
    ~ProcManagerFCFS();
    void getFcfsInfo();
    void getFcfsInfo(int pid);
    int getQueueSize();
private:
    vector <PCB> fcfsQueue;
    void run(PCB p);
    string getCommand();
};

//���̹�������
class ProcManager
{
private:
    // ��¼���õ�pid��
    int cpid;
    // ���ھ���״̬��pcb���������Ӧ�ò����ڣ���װ�������ڴ�
    vector<PCB*> active_pcb;
    // λ�ڵȴ�״̬��pcb
    vector<PCB*> waiting_pcb;
    // RR����
    RRQueue* rr_queue;
    ProcManagerFCFS* fcfsProcManager;
public:
    ProcManager();
    ProcManager(int n_size, int x_size);
    ~ProcManager();
    int getActiveNum();
    void kill();
    void kill(int pid);
    void run(string file_name);
    void ps();
    void ps(int pid);
    void scheduling();
};

#endif // !PROC_H

