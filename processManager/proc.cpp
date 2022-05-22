#include "proc.h"
#include "memtest.cpp"
using namespace std;


/*****************************************************************************************
 * RRQueue类的有关定义部分
 ****************************************************************************************/

RRQueue::RRQueue(){
    initCmdMap();
}

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
    // target->path = "./home/test";
    fcfs->addToQueue(target);
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
            ProcManager::getInstance().freePCB(pcb);
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
    // 剩余时间
    int time = TIME_SLICE;
    int pid;
    // cout << setw(WIDTH) << "Id" << setw(WIDTH) << "Time_need\n";
    // 循环到rr队列为空
    while (!this->rr_que.empty())
    {
        // 对每个pcb进行处理
        // auto it = this->rr_que.begin();
        int it = 0;
        // while (it != this->rr_que.end())
        while (it < this->rr_que.size())
        {
            WaitForSingleObject(pMutex,INFINITE);
            // PCB* cur_pcb = *it;
            PCB* cur_pcb = this->rr_que[it];
            pid = cur_pcb->id;
            // cout << "DEBUG_PID RRQueue_scheduling " << pid << endl;
            // cout << "DEBUG_PC " << cur_pcb->pc << endl;
            time = TIME_SLICE;
            // cout << setw(WIDTH) << cur_pcb->id << setw(WIDTH) << cur_pcb->time_need << endl;
            // 判断时间是否够完成一次循环
            cur_pcb->status = RUNNING;
            cur_pcb->slice_cnt++;
            exec(cur_pcb, time);
            if (ProcManager::getInstance().killed == pid)
            {
                // 程序死亡
                cout << "pid: " << pid << " is dead\n";
                ProcManager::getInstance().killed = -1;
            }
            else if (cur_pcb->status == DEAD)
            {
                // 程序死亡
                cout << "pid: " << cur_pcb->id << " is dead\n";
                ProcManager::getInstance().freePCB(cur_pcb);
                // it = this->rr_que.erase(it);
                this->rr_que.erase(rr_que.begin() + it);
            }
            else if (cur_pcb->status == BLOCKED)
            {
                // 从队列移除，加入到阻塞队列中
                // cout << "DEBUG BLOCKED" << endl;
                ProcManager::getInstance().block(cur_pcb,0);
                // it = this->rr_que.erase(it);
                this->rr_que.erase(rr_que.begin() + it);
            }
            else if (cur_pcb->slice_cnt >= MAX_CNT)
            {
                cur_pcb->pri = LOW_PRI;
                // 降级加入fcfs队列中,并从当前队列删除
                this->downLevel(cur_pcb,fcfs);
                // it = this->rr_que.erase(it);
                this->rr_que.erase(rr_que.begin() + it);
            }
            else
            {
                // cout << "DEBUG IT ++" << endl;
                it++;
            }
            // 维护队列
            ProcManager::getInstance().maintain(TIME_SLICE - time);
            ReleaseMutex(pMutex);
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
        cout << " pri: " << pcb->pri << endl;
    }
}

/**
 * 获取RR队列中指定pcb的信息
 * @param pid 指定pcb的id
 * @return 返回指向目标的指针
 */
PCB* RRQueue::getInfo(int pid)
{
    for (auto it=this->rr_que.begin(); it != rr_que.end(); it++)
    {
        PCB* pcb = *it;
        if (pcb->id == pid)
        {
            return pcb;
        }
    }
    return nullptr;
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
void ProcManagerFCFS::addToQueue(PCB *p){
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
            PCB *p = fcfsQueue.front();
            run(p);
            if(!fcfsQueue.empty()){
                auto it = fcfsQueue.begin();
                it = fcfsQueue.erase(it);
            }
        }
        return ;
    }
}

/*** 
 * @brief 获得下一条指令
 * @param {PCB} p
 * @return {string} command
 */
string ProcManagerFCFS::getCommand(PCB *p){
    string command = "";
    // cout << "DEBUG_PC " << p->pc << endl;
    char tmp = bmm->accessMemory(p->id,p->pc);
    p->pc += 1;
    if(tmp == '#'){
        Sleep(1000);
        cout << "end of file" << endl;
        return command;
    }
    while(tmp != '\0' && tmp != '#'){
        if(tmp == -1){
            command = "proc not found";
            return command;
        }
        command += tmp;
        tmp = bmm->accessMemory(p->id,p->pc);
        if(tmp == -1){
            command = "proc not found";
            return command;
        }
        p->pc += 1;
    }
    return command;
    // 这个函数是用来取出内存中的一条指令
    
    // return "WriteMemory 100 5";
}

string ProcManagerFCFS::splitCommand(string command){
    // 这个函数用来取出指令字部分
    int pos = command.find(' ');
    return command.substr(0,pos);
}



/*** 
 * @brief execute the programme,not written yet
 * @param {PCB} p
 * @param {PageMemoryManager*} m
 * @return {NULL}
 */
void ProcManagerFCFS::run(PCB *p){
    //把剩下的CPU时间跑完
    if(p->cpu_time != 0){
        Sleep(p->cpu_time);
        cout << "pid: " << p->id << " is using cpu for " << p->cpu_time << endl;
        p->cpu_time = 0;
    }
    while(true){
        string command = getCommand(p);
        if(command == ""){
            break ;
        }
        if(command == "proc not found"){
            return ;
        }
        string cmd = splitCommand(command);
        //剩下的是指令中的参数
        cout << endl << command << endl;
        command = command.substr(cmd.length() + 1,command.length());
        cout<<"[pid "<< p->id<<"] ";
        switch(this->commandMap[cmd]){
            case 0:
                writeMem(command,p->id);
                break;
            case 1:
                accessMem(command,p->id);
                break;
            case 2:
                useCPU(command);
                break;
            case 3:
                moveToWaiting(p->id);
                useIO(command);
                return ;
            default:
                break ;
        }
    }
    ProcManager::getInstance().freePCB(p);
    return ;
}

void ProcManagerFCFS::moveToWaiting(int pid){
    PCB *tmp = fcfsQueue.front();
    blocked.push_back(tmp);
    cout << "pid " << tmp->id << "is blocked" << endl;
    for(auto it = blocked.begin();it != blocked.end();it++){
        if((*it)->id == pid){
            it = blocked.erase(it);
            break;
        }
    }
    fcfsQueue.push_back(tmp);
    return ;
}

/*** 
 * @brief remove a process from the fcfs queue
 * @param {int} pid
 * @return {NULL}
 */
bool ProcManagerFCFS::removeProc(int pid){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        if((*it)->id == pid){
            ProcManager::getInstance().freePCB(*it);
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
    // for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
    //     cout << (*it)->id << " " << endl;
    // }
    for (PCB* pcb : fcfsQueue)
    {
        cout << "pid: " << pcb->id << " name:" << pcb->name ;
        cout << " pri: " << pcb->pri << endl;
    }
    return ;
}



/*** 
 * @brief get selected process information in the fcfs queue
 * @param {int} pid
 * @return 指向目标的指针
 */
PCB* ProcManagerFCFS::getFcfsInfo(int pid){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        if((*it)->id == pid){
            return *it;
        }
    }
    return nullptr;
}


/*** 
 * @brief get the number of processes in the fcfs queue
 * @param {NULL}
 * @return {NULL}
 */
int ProcManagerFCFS::getQueueSize(){
    return fcfsQueue.size();
}



/*** 
 * @brief 
 * @param {NULL}
 * @return {NULL}
 */
void ProcManagerFCFS::initCmdMap(){
    commandMap["WriteMemory"] = 0;
    commandMap["access"] = 1;
    commandMap["cpu"] = 2;
    commandMap["keyboard"] = 3;
    return ;
}



/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void ProcManagerFCFS::useCPU(string command){
    int time = atoi(command.c_str());
    cout << "use CPU " << time << endl;
    if(CPU){
        CPU = false;
        Sleep(time);
        CPU = true;
    }
    else{
        while(!CPU);
        CPU = false;
        Sleep(time);
        CPU = true;
    }
    return ;
}



/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void ProcManagerFCFS::useIO(string command){
    int time = atoi(command.c_str());
    cout << "IO Time" << " " << time << endl;
    if(IO){
        IO = false;
        Sleep(time);
        IO = true;
    }
    else{
        while(!IO);
        IO = false;
        Sleep(time);
        IO = true;
    }
    return ;
}



/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void ProcManagerFCFS::accessMem(string command,int pid){
    int addr = atoi(command.c_str());
    cout <<"access Memory at addr" << " " << addr << ", " << "the content is " << bmm->accessMemory(pid,addr) << endl;
    Sleep(1000);
    return ;
}


/*** 
 * @brief 
 * @param {string} command
 * @param {string} num
 * @param {int} number
 * @return {NULL}
 */
void ProcManagerFCFS::writeMem(string command,int pid){
    int pos = command.find(' ');
    string addr = command.substr(0,pos);
    // string num = command.substr(pos + 1,command.length());
    char tmp = command[command.length() - 1];
    // int number = atoi(num.c_str());
    int address = atoi(addr.c_str());
    cout << "write Memory at addr" << " " << addr << " with " << tmp << endl;
    bmm->writeMemory(address,tmp,pid);
    return ;
}

ProcManagerFCFS::ProcManagerFCFS(){
    initCmdMap();
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
    this->fcfsProcManager = new ProcManagerFCFS();
    pMutex = CreateMutex(NULL, FALSE, NULL);
    block_pcb.resize(DEV_NUM);
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
    srand(time(NULL));
    // 不会被降级的PCB
    for (int i = 0; i < n_size; i++)
    {
        PCB* new_pcb = new PCB;
        new_pcb->id = i;
        new_pcb->name = "normal";
        new_pcb->status = READY;
        new_pcb->pri = HIGH_PRI;
        new_pcb->slice_cnt = 0;
        new_pcb->time_need = rand() % 20 * 100;
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
        waiting_pcb.push_back(new_pcb);
    }
    this->cpid = n_size+x_size;

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
        killed = pid;
        this->maintain(0);
        return;
    }

    // 在等待队列里面寻找进程
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // 找到了，删除它
        if ((*it)->id == pid)
        {
            (*it)->status = DEAD;
            // delete *it;
            this->freePCB(*it);
            this->waiting_pcb.erase(it);
            printf("[%ld]Waiting pid=%d is killed.\n", clock() - system_start, pid);
            return;
        }
    }
    // 在阻塞队列里面寻找进程
    for (int i=0; i < DEV_NUM; i++)
    {
        for (auto it=this->block_pcb[i].begin(); it < this->block_pcb[i].end(); it++)
        {
            // 找到了，删除它
            if ((*it)->id == pid)
            {
                (*it)->status = DEAD;
                // delete *it;
                this->freePCB(*it);
                this->block_pcb[i].erase(it);
                printf("[%ld]Block pid=%d is killed.\n", clock() - system_start, pid);
                return;
            }
        }
    }
    printf("[%ld]Can't find pid=%d.\n", clock() - system_start, pid);
}

/**
 * 列出当前所有存在的pcb
 */
void ProcManager::ps()
{  
    cout << "Waiting:" << this->waiting_pcb.size() << endl;
    for (PCB* pcb : waiting_pcb)
    {
        cout << "pid: " << pcb->id << " name:" << pcb->name ;
        cout << " pri: " << pcb->pri << endl;
    }
    cout << "Running: " << this->getActiveNum() << endl;
    cout << "RR: " << this->rr_queue->getSize() << endl;
    cout << "FCFS: " << this->fcfsProcManager->getQueueSize() << endl;
    for (int i=0; i < DEV_NUM; i++)
    {
        cout << "Block" << i << ": " << block_pcb[i].size() << endl;
    }
    rr_queue->getInfo();
    fcfsProcManager->getFcfsInfo();
}

/**
 * 列出指定的pcb
 * @param pid 指定
 */
void ProcManager::ps(int pid)
{
    PCB* target=nullptr;
    for (auto pcb:this->waiting_pcb)
    {
        if (pcb->id == pid)
        {
            target = pcb;
            break;
        }
    }
    if(target == nullptr)
    {
        target = rr_queue->getInfo(pid);
        if (target == nullptr)
        {
            target = fcfsProcManager->getFcfsInfo(pid);
            if (target == nullptr)
            {
                for(int i=0; i < DEV_NUM; i++)
                {
                    for (auto pcb:this->block_pcb[i])
                    {
                        if (pcb->id == pid)
                        {
                            target = pcb;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (target != nullptr)
    {
        cout << "pid: " << target->id << " name:" << target->name ;
        cout << " pri: " << target->pri << endl;
    }
    else
    {
        cout << "Can't find pid " << pid << endl;
    }
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
    this->cpid = (this->cpid + 1) % 65536;
    new_pcb->name = file_name;
    new_pcb->status = NEW;
    new_pcb->pri = HIGH_PRI;
    new_pcb->time_need = 1888;
    new_pcb->slice_cnt = 0;
    PCB* pcb = new_pcb;

    // 判断是否需要加入到等待队列
    if (pcb->pri == HIGH_PRI && this->rr_queue->getSize() < MAX_PROC)
    {
        pcb->status = READY;
        this->rr_queue->addPCB(pcb);
        printf("[%ld]Pid=%d is running.\n", clock() - system_start, pcb->id);
    }
    else if (pcb->pri == LOW_PRI && this->fcfsProcManager->getQueueSize() < MAX_PROC)
    {
        pcb->status = READY;
        this->fcfsProcManager->addToQueue(pcb);
        printf("[%ld]Pid=%d is running.\n", clock() - system_start, pcb->id);
    }
    else
    {
        this->waiting_pcb.push_back(pcb);
        printf("[%ld]Pid=%d is waiting.\n", clock() - system_start, pcb->id);
    }
}

/**
 * @brief 正式的run
 * 
 * @param pcb 
 */
void ProcManager::run(PCB* pcb)
{
    WaitForSingleObject(pMutex,INFINITE);
    if (bmm->createProcess(*pcb) == -1)
    {
        ReleaseMutex(pMutex);
        return;
    }
    // 判断是否需要加入到等待队列
    if (pcb->pri == HIGH_PRI && this->rr_queue->getSize() < MAX_PROC)
    {
        pcb->status = READY;
        this->rr_queue->addPCB(pcb);
        printf("[%ld]Pid=%d is running.\n", clock() - system_start, pcb->id);
    }
    else if (pcb->pri == LOW_PRI)
    {
        pcb->status = READY;
        this->fcfsProcManager->addToQueue(pcb);
        printf("[%ld]Pid=%d is running.\n", clock() - system_start, pcb->id);
    }
    else
    {
        this->waiting_pcb.push_back(pcb);
        printf("[%ld]Pid=%d is waiting.\n", clock() - system_start, pcb->id);
    }
    ReleaseMutex(pMutex);
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
 * @brief 释放pcb占用的空间
 * 
 * @param target 指向释放pcb的指针
 * @return true 成功
 * @return false 失败
 */
bool ProcManager::freePCB(PCB* target)
{
    bmm->freeProcess(*target);
    delete target;
    return true;
}

/**
 * @brief 维护调度队列
 * 
 */
void ProcManager::maintain(int time_pass)
{
    // 维护阻塞队列
    for (int i=0; i < DEV_NUM; i++)
    {
        auto it = block_pcb[i].begin();
        while(time_pass != 0 && it != block_pcb[i].end())
        {
            PCB* pcb = *it;
            if (pcb->block_time > time_pass)
            {
                pcb->block_time -= time_pass;
                time_pass = 0;
            }
            else
            {
                pcb->block_time = 0;
                time_pass -= pcb->block_time;
                pcb->status = NEW;
                waiting_pcb.insert(waiting_pcb.begin(), pcb);
                it = block_pcb[i].erase(it);
            }
        }
    }

    // 维护等待队列
    auto it = waiting_pcb.begin();
    bool free1 = rr_queue->getSize() < MAX_PROC;
    while (it != waiting_pcb.end() && free1)
    {
        PCB* pcb = *it;
        // cout  << "WAITING: "<< (pcb->pri == HIGH_PRI) << "  " << (pcb->pri == LOW_PRI);
        if (pcb->pri == HIGH_PRI && free1)
        {
            rr_queue->addPCB(pcb);
            pcb->status = READY;
            printf("Pid %d is running.\n", pcb->id); 
            it = waiting_pcb.erase(it);
        }
        else if (pcb->pri == LOW_PRI)
        {
            fcfsProcManager->addToQueue(pcb);
            pcb->status = READY;
            printf("Pid %d is running.\n", pcb->id); 
            it = waiting_pcb.erase(it);
        }
        else
        {
            it++;
        }
        free1 = rr_queue->getSize() < MAX_PROC;
    }

    // // 维护RR队列
    // if (rr_queue->getSize() < MAX_PROC && waiting_pcb.size() > 0)
    // {
    //     for (auto it = waiting_pcb.begin(); it != waiting_pcb.end(); it++)
    //     {
    //         PCB* new_pcb = *it;
    //         if (new_pcb->pri == HIGH_PRI)
    //         {
    //             waiting_pcb.erase(it);
    //             rr_queue->addPCB(new_pcb);
    //             printf("[%ld]Pid %d is running.\n", clock() - system_start, new_pcb->id);
    //             break;
    //         }
    //     }
    // }
    // // 维护fcfs队列
    // if (fcfsProcManager->getQueueSize() < MAX_PROC && waiting_pcb.size() > 0)
    // {
    //     for (auto it = waiting_pcb.begin(); it != waiting_pcb.end(); it++)
    //     {
    //         PCB* new_pcb = *it;
    //         if (new_pcb->pri == LOW_PRI)
    //         {
    //             waiting_pcb.erase(it);
    //             fcfsProcManager->addToQueue(new_pcb);
    //             printf("[%ld]Pid %d is running.\n", clock() - system_start, new_pcb->id);
    //             break;
    //         }
    //     }
    // }
}

/**
 * @brief 可以返回单例的函数
 * 
 * @return ProcManager& 
 */
ProcManager& ProcManager::getInstance() 
{
    // 测试用
    static ProcManager instance;
    // static ProcManager instance;
    return instance;
}

/**
 * @brief 返回一个可用的pid
 * 
 * @return int 可用的pid
 */
int ProcManager::getAvailableId()
{
    int id = this->cpid;
    this->cpid = (this->cpid + 1) % 65536;
    return id;
}

void ProcManager::block(PCB* pcb, int dev)
{
    // cout << pcb->id << endl;
    block_pcb[dev].push_back(pcb);
}

/*** 
 * @brief 
 * @param {NULL}
 * @return {NULL}
 */
void RRQueue::initCmdMap(){
    commandMap["WriteMemory"] = 0;
    commandMap["access"] = 1;
    commandMap["cpu"] = 2;
    commandMap["IO"] = 3;
    commandMap["keyboard"] = 4;
    return ;
}



/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void RRQueue::useCPU(string command){
    int time = atoi(command.c_str());
    cout << "use CPU " << time << endl;
    if(CPU){
        CPU = false;
        Sleep(time);
        CPU = true;
    }
    else{
        while(!CPU);
        CPU = false;
        Sleep(time);
        CPU = true;
    }
    return ;
}



/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void RRQueue::useIO(string command){
    int time = atoi(command.c_str());
    cout << "IO Time" << " " << time << endl;
    if(IO){
        IO = false;
        Sleep(time);
        IO = true;
    }
    else{
        while(!IO);
        IO = false;
        Sleep(time);
        IO = true;
    }
    return ;
}

/*** 
 * @brief 
 * @param {string} command
 * @return {NULL}
 */
void RRQueue::accessMem(string command,int pid){
    int addr = atoi(command.c_str());
    // cout << "DEBUG_PID RRQueue::accessMem " << pid << endl;
    cout <<"access Memory at addr" << " " << addr << ", " << "the content is " << bmm->accessMemory(pid,addr) << endl;
    Sleep(TIME_SLICE * ALPHA);
    return ;
}

/*** 
 * @brief 
 * @param {string} command
 * @param {string} num
 * @param {int} number
 * @return {NULL}
 */
void RRQueue::writeMem(string command,int pid){
    int pos = command.find(' ');
    string addr = command.substr(0,pos);
    // string num = command.substr(pos + 1,command.length());
    char tmp = command[command.length() - 1];
    // int number = atoi(num.c_str());
    int address = atoi(addr.c_str());
    cout << "write Memory at addr" << " " << addr << " with " << tmp << endl;
    bmm->writeMemory(address,tmp,pid);
    Sleep(TIME_SLICE * ALPHA);
    return ;
}

/*** 
 * @brief 获得下一条指令
 * @param {PCB} p
 * @return {string} command
 */
string RRQueue::getCommand(PCB *p){
    string command = "";
    // cout << "DEBUG_PC " << p->pc << endl;
    // cout << "DEBUG_PID getCommand" << p->id << endl;
    char tmp = bmm->accessMemory(p->id,p->pc);
    p->pc += 1;
    if(tmp == '#'){
        cout << "end of file" << endl;
        return command;
    }
    while(tmp != '\0' && tmp != '#'){
        command += tmp;
        tmp = bmm->accessMemory(p->id,p->pc);
        p->pc += 1;
    }
    return command;
}

string RRQueue::splitCommand(string command)
{
    // 这个函数用来取出指令字部分
    int pos = command.find(' ');
    return command.substr(0,pos);
}

void RRQueue::exec(PCB *p, int &time)
{
    string command;
    string cmd = "cpu";
    // 零表示上一条cpu占用指令跑完了，需要取新指令
    if (p->cpu_time == 0)
    {
        // 取指令
        command = getCommand(p);
        if(command == ""){
            p->status = DEAD;
            return ;
        }
        cmd = splitCommand(command);
        //剩下的是指令中的参数
        // cout << endl << command << endl;
        command = command.substr(cmd.length() + 1,command.length());
        // cout << endl << commandMap[cmd] << endl;
        if (cmd == "cpu")
        {
            p->cpu_time = atoi(command.c_str());
        }
    }
    switch(this->commandMap[cmd]){
        case 0:
            writeMem(command, p->id);
            time = 0;
            break;
        case 1:
            accessMem(command, p->id);
            time = 0;
            break;
        case 2:
            cout << "pid: " << p->id << " is using cpu for " << TIME_SLICE - time << endl;
            if (p->cpu_time >= time)
            {
                time = 0;
                p->cpu_time -= TIME_SLICE;
                Sleep(TIME_SLICE * ALPHA);
            }
            else
            {
                time -= p->cpu_time;
                p->cpu_time = 0;
                Sleep(p->cpu_time * ALPHA);
            }
            break;
        case 4:
            // 键盘阻塞
            p->status = BLOCKED;
            p->block_time = atoi(command.c_str());
            printf("Pid %d block!.\n", p->id);
            Sleep(TIME_SLICE * ALPHA);
            time = 0;
            break;
        default:
            return ;
    }
}


// int main()
// {
//     // RRQueue test_queue(5,2);
//     system_start = clock();
//     cout << setiosflags(ios::left);
//     printf("[%ld]This is a test\n", clock() - system_start);


//     cout << "ps all" << endl;
//     ProcManager::getInstance().ps();
//     cout << "ps 3" << endl;
//     ProcManager::getInstance().ps(3);
//     cout <<"kill test\n";
//     ProcManager::getInstance().kill(3);
//     ProcManager::getInstance().ps();
//     cout <<"run test\n";
//     ProcManager::getInstance().run("run_test");
//     ProcManager::getInstance().ps();
//     ProcManager::getInstance().scheduling();
//     ProcManager::getInstance().ps();


//     // system("pause");
//     return 0;
// }