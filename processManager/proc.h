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
#define WIDTH 10
// ʱ��Ƭ��С
#define TIME_SLICE 200
// �����ȼ�
#define HIGH_PRI 0
// �����ȼ�
#define LOW_PRI 1
// ���ռ�õ�ʱ��Ƭ����
#define MAX_CNT 10

// PCB�Ľṹ��
typedef struct 
{
   // pid
   int id;
   // ���ȼ�
   int pri;
   // ʹ�ù���ʱ��Ƭ����
   int slice_cnt;
   // Ԥ�ƻ���Ҫ��ʱ��
   int time_need;
} PCB;

// CPU�Ƿ���ʹ�ã�����չΪ����
bool CPU[2];
// IO�豸�Ƿ���ʹ�ã�����չΪ����
bool IO[2];

// ȫ�ֱ���
// rr����
// static queue<PCB*> rr_que;
// ����Ŀǰ��Ծ��PCB
static vector<PCB*> active_pcb;
// �������еĿ�ʼʱ��
clock_t system_start;

static queue <PCB> FCFS;

/**
 * @brief RR������
 */
class RRQueue {
private:
    //Ŀǰ������PCB������
    int size;
    //rr���б���
    queue<PCB*> rr_que;
public:
    /**
     * @brief RR���е�һ�㹹��
     * @param void
     */
    RRQueue();
    /**
     * @brief �����ڲ�����ʹ�ã���������Ĵ�С�������
     * @param n_size �����в��ᱻ�����ĸ���
     * @param x_size �������ĸ���
     */
    RRQueue(int n_size, int x_size);
    /**
     * @brief ��������
     */
    ~RRQueue();
    /**
     * @brief ��ѯĿǰ���еĳ���
     * @return ���г���
     */
    int getSize();
};

#endif // !PROC_H

