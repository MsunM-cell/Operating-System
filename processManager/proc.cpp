/**
 * @file testRR.cpp
 * @brief 有关于一级队列的一些尝试
 * @date 2022-03-25
 */
#include "proc.h"
using namespace std;


/*****************************************************************************************
 * RRQueue类的有关定义部分
 ****************************************************************************************/

/**
 * @brief 析构函数
 */
RRQueue::~RRQueue()
{
    cout << "RR queue takes a nap.\n";
}

/**
 * @brief 查询目前队列的长度
 * @return 队列长度
 */
int RRQueue::getSize()
{
    return (int)(this->rr_que.size());
}

/**
 * @brief 降级操作
 * @param target 被降级的PCB
 */
void RRQueue::downLevel(PCB* target,ProcManagerFCFS* fcfs)
{
    fcfs->addToQueue(*target);
    printf("[%ld]Pid %d down to fcfs.\n", clock() - system_start, target->id);
}

/**
 * 向RR队列中加入一个PCB
 * @param target 指向要加入的PCB
 * @return 成功则返回true
 */
bool RRQueue::addPCB(PCB *target)
{
    rr_que.push_back(target);
    return true;
}

/**
 * 移除一个pcb,并将其占用的内存释放
 * @param pid 将要移除的pcb的id
 * @return 成功则返回true
 */
bool RRQueue::removePCB(int pid)
{
    for (auto it = this->rr_que.begin(); it != this->rr_que.end(); it++)
    {
        PCB* pcb = *it;
        if (pcb->id == pid)
        {
            pcb->status = DEAD;
            delete pcb;
            this->rr_que.erase(it);
            return true;
        }
    }
    return false;
}

/**
 * @brief 调度主算法
 * @return 正常结束返回0
 */
int RRQueue::scheduling(ProcManagerFCFS* fcfs)
{
    // cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // 循环到rr队列为空
    while (!this->rr_que.empty())
    {
        // 对每个pcb进行处理
        for (auto it = this->rr_que.begin(); it < this->rr_que.end(); it++)
        {
            PCB* cur_pcb = *it;
            // cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // 判断时间是否够完成一次循环
            cur_pcb->status = RUNNING;
            if (cur_pcb->time_need > TIME_SLICE)
            {
                // 模拟服务过程
                Sleep(TIME_SLICE);
                // 时间片到
                cur_pcb->status = READY;
                cur_pcb->time_need -= TIME_SLICE;
                printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                       cur_pcb->time_need);
                // 判断一下是否使用了过多的时间片,是则降级
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT)
                {
                    cur_pcb->pri = LOW_PRI;
                    // 降级加入fcfs队列中,并从当前队列删除
                    this->downLevel(cur_pcb,fcfs);
                    it = this->rr_que.erase(it);
                }
            }
            else
            {
                // 需要的时间小于完整的时间片，完成后从队列中删除该项
                Sleep(cur_pcb->time_need);
                // cur_pcb->time_need = -1;
                cur_pcb->status = DEAD;
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
                delete cur_pcb;
                it = this->rr_que.erase(it);
            }
        }
    }
    return 0;
}

/**
 * 获取RR队列中所有pcb的信息
 * @return 迭代器头指针
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
 * 获取RR队列中指定pcb的信息
 * @param pid 指定pcb的id
 * @return 指向其的迭代器，没找到就返回空
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
 * ProcManagerFCFS类的有关定义部分
 ****************************************************************************************/

/*** 
 * @brief destructor
 * @param {NULL}
 * @return {NULL}
 */
ProcManagerFCFS::~ProcManagerFCFS(){
    cout << "FCFS proc manager is terminated" << endl;
}


/*** 
 * @brief add an process into the fcfs queue
 * @param {PCB} p
 * @return {NULL}
 */
void ProcManagerFCFS::addToQueue(PCB p){
    fcfsQueue.push_back(p);
}


/*** 
 * @brief run processes using an fcfs algorithm
 * @param {PageMemoryManager*} m
 * @return {NULL}
 */
void ProcManagerFCFS::runProcManager(){
    while(true){
        while(!fcfsQueue.empty()){
            PCB p = fcfsQueue.front();
            //该函数是执行函数，暂时未定
            run(p);
            // FIXME 无用pcb的及时删除， delete
            auto it = fcfsQueue.begin();
            it = fcfsQueue.erase(it);
        }
        return ;
    }
}

/*** 
 * @brief not written yet
 * @param {NULL}
 * @return {NULL}
 */
string ProcManagerFCFS::getCommand(){
    return "aaa";
}



/*** 
 * @brief execute the programme,not written yet
 * @param {PCB} p
 * @param {PageMemoryManager*} m
 * @return {NULL}
 */
void ProcManagerFCFS::run(PCB p){
    // 因为还没定文件格式，run函数暂时没有办法写
    cout << "process " << p.id << " is running," << " will use " << p.time_need << " ms."<< endl;
    Sleep(p.time_need);
    cout << "process " << p.id << " is terminated" << endl;
    return ;
}



/*** 
 * @brief remove a process from the fcfs queue
 * @param {int} pid
 * @return {NULL}
 */
bool ProcManagerFCFS::removeProc(int pid){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        if(it->id == pid){
            // FIXME pcb也得删
            it = fcfsQueue.erase(it);
            return true;
        }
    }
    cout << "no such pid" << endl;
    return false;
}



/*** 
 * @brief get all active process information in the fcfs queue
 * @param {NULL}
 * @return {NULL}
 */
void ProcManagerFCFS::getFcfsInfo(){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        cout << it->id << " " << endl;
    }
    return ;
}



/*** 
 * @brief get selected process information in the fcfs queue
 * @param {int} pid
 * @return {NULL}
 */
void ProcManagerFCFS::getFcfsInfo(int pid){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        if(it->id == pid){
            cout << it->id << " " << endl;
            return ;
        }
    }
    cout << "no such process" << endl;
    return ;
}


/*** 
 * @brief get the number of processes in the fcfs queue
 * @param {NULL}
 * @return {NULL}
 */
int ProcManagerFCFS::getQueueSize(){
    return fcfsQueue.size();
}



/*****************************************************************************************
 * ProcManager类的有关定义部分
 ****************************************************************************************/

/**
 * @brief Construct a new Proc Manager:: Proc Manager object
 * ProcManager的构造函数
 */
ProcManager::ProcManager()
{
    this->cpid = 0;
    this->rr_queue = new RRQueue();
    cout << "ProcManager is running!\n";
}

/**
 * test
 * @param x
 * @param y
 */
ProcManager::ProcManager(int n_size, int x_size)
{
    this->cpid = 0;
    this->rr_queue = new RRQueue();
    this->fcfsProcManager = new ProcManagerFCFS();
    cout << "ProcManager is running!\n";
    // 不会被降级的PCB
    for (int i = 0; i < n_size; i++)
    {
        PCB* new_pcb = new PCB;
        new_pcb->id = i;
        new_pcb->name = "normal";
        new_pcb->status = READY;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 30 * 100;
        active_pcb.push_back(new_pcb);
    }
    // 降级测试
    for (int i = 0; i < x_size; i++)
    {
        PCB* new_pcb = new PCB;
        new_pcb->id = n_size + i;
        new_pcb->name ="large";
        new_pcb->status = READY;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 10 * 100 + TIME_SLICE * MAX_CNT;
        active_pcb.push_back(new_pcb);
    }
    this->cpid = 7;

    // 向rr添加pcb，之后可重用
    for (PCB* pcb : this->active_pcb)
    {
        this->rr_queue->addPCB(pcb);
    }
}

/**
 * @brief ProcManager的析构函数
 */
ProcManager::~ProcManager()
{
    cout << "ProcManager is over!\n";
}

/**
 * 获取当前活跃的进程个数
 * @return 进程个数
 */
int ProcManager::getActiveNum()
{
    return rr_queue->getSize() + fcfsProcManager->getQueueSize();
}


/**
 * 杀掉一个进程
 * @param pid 进程的id
 */
void ProcManager::kill(int pid)
{
    // 找到的标志
    bool is_found = false;
    // 在两个队列里面找
    is_found = this->rr_queue->removePCB(pid) || this->fcfsProcManager->removeProc(pid);
    if (is_found)
    {
        printf("[%ld]Active pid=%d is killed.\n", clock() - system_start, pid);
        return;
    }

    // 在等待队列里面寻找进程
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // 找到了，删除它
        if ((*it)->id == pid)
        {
            (*it)->status = DEAD;
            delete *it;
            this->waiting_pcb.erase(it);
            printf("[%ld]Waiting pid=%d is killed.\n", clock() - system_start, pid);
        }
    }
    printf("[%ld]Can't find pid=%d.\n", clock() - system_start, pid);
}

/**
 * 列出当前所有存在的pcb
 */
void ProcManager::ps()
{
    cout << "All: " << this->getActiveNum() << endl;
    cout << "RR: " << this->rr_queue->getSize() << endl;
    cout << "FCFS: " << this->fcfsProcManager->getQueueSize() << endl;
    rr_queue->getInfo();
}

/**
 * 列出指定的pcb
 * @param pid 指定
 */
void ProcManager::ps(int pid)
{
    rr_queue->getInfo(pid);
}

/**
 * 启动一个进程
 * @param file_name 要启动的文件名
 */
void ProcManager::run(string file_name)
{
    // 从其他模块获取文件的有关信息，这里模拟一下
    PCB* new_pcb = new PCB;
    new_pcb->id = this->cpid;
    new_pcb->name = file_name;
    new_pcb->status = NEW;
    new_pcb->pri = HIGH_PRI;
    new_pcb->time_need = 1888;
    new_pcb->slice_cnt = 0;
    this->cpid = (this->cpid + 1) % 65536;
    PCB* pcb = new_pcb;

    // 判断是否需要加入到等待队列
    if (this->getActiveNum() < MAX_PROC)
    {
        pcb->status = READY;
        if (pcb->pri == HIGH_PRI)
        {
            this->rr_queue->addPCB(pcb);
        }
        else if (pcb->pri == LOW_PRI)
        {
            this->fcfsProcManager->addToQueue(*pcb);
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
 * 开始调度
 */
void ProcManager::scheduling()
{
    rr_queue->scheduling(fcfsProcManager);
    fcfsProcManager->runProcManager();
}

/**
 * @brief 可以返回单例的函数
 * 
 * @return ProcManager& 
 */
ProcManager& ProcManager::getInstance() 
{
    // 测试用
    static ProcManager instance(5,2);
    // static ProcManager instance;
    return instance;
}

int main()
{
    // RRQueue test_queue(5,2);
    // 系统开始计时，实际应该更早
    system_start = clock();
    cout << setiosflags(ios::left);
    printf("[%ld]This is a RR test\n", clock() - system_start);


    cout << "ps all" << endl;
    ProcManager::getInstance().ps();
    cout << "ps 3" << endl;
    ProcManager::getInstance().ps(3);
    cout <<"kill test\n";
    ProcManager::getInstance().kill(3);
    ProcManager::getInstance().ps();
    cout <<"run test\n";
    ProcManager::getInstance().run("run_test");
    ProcManager::getInstance().ps();
    ProcManager::getInstance().scheduling();
    ProcManager::getInstance().ps();


    // system("pause");
    return 0;
}