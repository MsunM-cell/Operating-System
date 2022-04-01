/**
 * @file testRR.cpp
 * @brief �й���һ�����е�һЩ����
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/*****************************************************************************************
 * RRQueue����йض��岿��
 ****************************************************************************************/

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
 * ��RR�����м���һ��PCB
 * @param target ָ��Ҫ�����PCB
 * @return �ɹ��򷵻�true
 */
bool RRQueue::addPCB(PCB *target)
{
    rr_que.push_back(target);
    return true;
}

/**
 * �Ƴ�һ��pcb,������ռ�õ��ڴ��ͷ�
 * @param pid ��Ҫ�Ƴ���pcb��id
 * @return �ɹ��򷵻�true
 */
bool RRQueue::removePCB(int pid)
{
    for (auto it = this->rr_que.begin(); it != this->rr_que.end(); it++)
    {
        if ((*it)->id == pid)
        {
            delete *it;
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
    // cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // ѭ����rr����Ϊ��
    while (!this->rr_que.empty())
    {
        // ��ÿ��pcb���д���
        for (auto it = this->rr_que.begin(); it < this->rr_que.end(); it++)
        {
            PCB* cur_pcb = *it;
            // cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
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
                delete cur_pcb;
                it = this->rr_que.erase(it);
            }
        }
    }
    return 0;
}

/**
 * ��ȡRR����������pcb����Ϣ
 * @return ������ͷָ��
 */
void RRQueue::getInfo()
{
    for (PCB* pcb : rr_que)
    {
        cout << "pid: " << pcb->id << " name:" << pcb->name ;
        cout << " pri: " << pcb->pri << " need: " << pcb->time_need << endl;
    }
}

/**
 * ��ȡRR������ָ��pcb����Ϣ
 * @param pid ָ��pcb��id
 * @return ָ����ĵ�������û�ҵ��ͷ��ؿ�
 */
void RRQueue::getInfo(int pid)
{
    for (auto it=this->rr_que.begin(); it != rr_que.end(); it++)
    {
        PCB* pcb = *it;
        if (pcb->id == pid)
        {
            cout << "pid: " << pcb->id << " name:" << pcb->name ;
            cout << " pri: " << pcb->pri << " need: " << pcb->time_need << endl;
            return;
        }
    }
    cout << "Can't find pid " << pid << endl;
}

/*****************************************************************************************
 * ProcManager����йض��岿��
 ****************************************************************************************/

/**
 * @brief ProcManager�Ĺ��캯��
 */
ProcManager::ProcManager()
{
    this->cpid = 0;
    this->rr_queue = new RRQueue();
    cout << "ProcManager is running!\n";
}

/**
 * ������
 * @param x
 * @param y
 */
ProcManager::ProcManager(int n_size, int x_size)
{
    this->cpid = 0;
    this->rr_queue = new RRQueue();
    cout << "ProcManager is running!\n";
    // ���ᱻ������PCB
    for (int i = 0; i < n_size; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = i;
        new_pcb->name = "normal";
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 30 * 100;
        active_pcb.push_back(new_pcb);
    }
    // ��������
    for (int i = 0; i < x_size; i++)
    {
        PCB* new_pcb = (PCB*) malloc(sizeof(PCB));
        new_pcb->id = n_size + i;
        new_pcb->name ="large";
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 10 * 100 + TIME_SLICE * MAX_CNT;
        active_pcb.push_back(new_pcb);
    }
    this->cpid = 7;

    // ��rr���pcb��֮�������
    for (PCB* pcb : this->active_pcb)
    {
        this->rr_queue->addPCB(pcb);
    }
}

/**
 * @brief ProcManager����������
 */
ProcManager::~ProcManager()
{
    cout << "ProcManager is over!\n";
}

/**
 * ��ȡ��ǰ��Ծ�Ľ��̸���
 * @return ���̸���
 */
int ProcManager::getActiveNum()
{
    return rr_queue->getSize();
    // TODO fcfs���Ӻ�:
    //  return rr_queue->getSize() + fcfs����;
}

/**
 * ����ȫɱ�ˣ�ʵ���ã�
 */
void ProcManager::kill()
{
    for (PCB* pcb : this->active_pcb)
    {
        delete pcb;
    }
}

/**
 * ɱ��һ������
 * @param pid ���̵�id
 */
void ProcManager::kill(int pid)
{
    // �ҵ��ı�־
    bool is_found = false;
    // ����������������
    // TODO ��fcfs�ԽӺ�Ϊ��
    //  is_found = this->rr_queue->removePCB(pid) || ��fcfs�ķ��ؽ����
    is_found = this->rr_queue->removePCB(pid);
    if (is_found)
    {
        printf("[%ld]Active pid=%d is killed.\n", clock() - system_start, pid);
        return;
    }

    // �ڵȴ���������Ѱ�ҽ���
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // �ҵ��ˣ�ɾ����
        if ((*it)->id == pid)
        {
            delete *it;
            this->waiting_pcb.erase(it);
            printf("[%ld]Waiting pid=%d is killed.\n", clock() - system_start, pid);
        }
    }
    printf("[%ld]Can't find pid=%d.\n", clock() - system_start, pid);
}

/**
 * �г���ǰ���д��ڵ�pcb
 */
void ProcManager::ps()
{
    cout << "All: " << this->getActiveNum() << endl;
    cout << "RR: " << this->rr_queue->getSize() << endl;
    rr_queue->getInfo();
}

/**
 * �г�ָ����pcb
 * @param pid ָ��
 */
void ProcManager::ps(int pid)
{
    rr_queue->getInfo(pid);
}

/**
 * ����һ������
 * @param file_name Ҫ�������ļ���
 */
void ProcManager::run(string file_name)
{
    // ������ģ���ȡ�ļ����й���Ϣ������ģ��һ��
    PCB* new_pcb = new PCB;
    new_pcb->id = this->cpid;
    new_pcb->name = file_name;
    new_pcb->pri = HIGH_PRI;
    new_pcb->time_need = 1888;
    new_pcb->slice_cnt = 0;
    this->cpid = (this->cpid + 1) % 65536;
    PCB* pcb = new_pcb;

    // �ж��Ƿ���Ҫ���뵽�ȴ�����
    if (this->getActiveNum() < MAX_PROC)
    {
        if (pcb->pri == HIGH_PRI)
        {
            this->rr_queue->addPCB(pcb);
        }
        else if (pcb->pri == LOW_PRI)
        {
            // ����fcfs����
        }
        printf("[%ld]Pid=%d is running.\n", clock() - system_start, pcb->id);
    }
    else
    {
        this->waiting_pcb.push_back(pcb);
        printf("[%ld]Pid=%d is waiting.\n", clock() - system_start, pcb->id);
    }
}

/**
 * ��ʼ����
 */
void ProcManager::scheduling()
{
    rr_queue->scheduling();
}

int main()
{
    // RRQueue test_queue(5,2);
    // ϵͳ��ʼ��ʱ��ʵ��Ӧ�ø���
    system_start = clock();
    cout << setiosflags(ios::left);
    printf("[%ld]This is a RR test\n", clock() - system_start);

    ProcManager proc_manager(5,2);
    cout << "ps all" << endl;
    proc_manager.ps();
    cout << "ps 3" << endl;
    proc_manager.ps(3);
    cout <<"kill test\n";
    proc_manager.kill(3);
    proc_manager.ps();
    cout <<"run test\n";
    proc_manager.run("run_test");
    proc_manager.ps();
    proc_manager.scheduling();
    proc_manager.ps();

    proc_manager.kill();
    return 0;
}