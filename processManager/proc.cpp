#include "proc.h"
#include "memtest.cpp"
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
void RRQueue::downLevel(PCB* target,ProcManagerFCFS* fcfs)
{
    target->path = "./home/test";
    fcfs->addToQueue(target);
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
 * @brief �������㷨
 * @return ������������0
 */
int RRQueue::scheduling(ProcManagerFCFS* fcfs)
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
            cur_pcb->status = RUNNING;
            if (cur_pcb->time_need > TIME_SLICE)
            {
                // ģ��������
                Sleep(TIME_SLICE);
                // ʱ��Ƭ��
                cur_pcb->status = READY;
                cur_pcb->time_need -= TIME_SLICE;
                // printf("[%ld]Pid %d time out! Still need %d.\n", clock() - system_start, cur_pcb->id,
                //        cur_pcb->time_need);
                // �ж�һ���Ƿ�ʹ���˹����ʱ��Ƭ,���򽵼�
                cur_pcb->slice_cnt++;
                if (cur_pcb->slice_cnt == MAX_CNT)
                {
                    cur_pcb->pri = LOW_PRI;
                    // ��������fcfs������,���ӵ�ǰ����ɾ��
                    this->downLevel(cur_pcb,fcfs);
                    it = this->rr_que.erase(it);
                }
            }
            else
            {
                // ��Ҫ��ʱ��С��������ʱ��Ƭ����ɺ�Ӷ�����ɾ������
                Sleep(cur_pcb->time_need);
                // cur_pcb->time_need = -1;
                cur_pcb->status = DEAD;
                printf("[%ld]Pid %d time out! No time need.\n", clock() - system_start, cur_pcb->id);
                // delete cur_pcb;
                ProcManager::getInstance().freePCB(cur_pcb);
                it = this->rr_que.erase(it);
            }

            // ά������
            ProcManager::getInstance().maintain();
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
 * @return ����ָ��Ŀ���ָ��
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
 * ProcManagerFCFS����йض��岿��
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
            //�ú�����ִ�к�������ʱδ��
            // FIXME ����ѭ����������ע�͵���
            p->size = 4096;
            p->pc = 0;
            bmm->createProcess(*p);
            run(p);
            Sleep(p->time_need);
            // cout << "pid:" << p->id << "shut in fcfs.\n";
            ProcManager::getInstance().freePCB(p);
            auto it = fcfsQueue.begin();
            it = fcfsQueue.erase(it);
        }
        return ;
    }
}

/*** 
 * @brief �����һ��ָ��
 * @param {PCB} p
 * @return {string} command
 */
string ProcManagerFCFS::getCommand(PCB *p){
    string command = "";
    char tmp = bmm->accessMemory(p->id,p->pc);
    p->pc += 1;
    if(tmp == '#'){
        Sleep(1000);
        cout << "end of file" << endl;
        return command;
    }
    while(tmp != '\0' && tmp != '#'){
        command += tmp;
        tmp = bmm->accessMemory(p->id,p->pc);
        p->pc += 1;
    }
    return command;
    // �������������ȡ���ڴ��е�һ��ָ��
    
    // return "WriteMemory 100 5";
}

string ProcManagerFCFS::splitCommand(string command){
    // �����������ȡ��ָ���ֲ���
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
    // Ŀǰ���ǵ�һ���ڴ�ӿ���
    while(true){
        string command = getCommand(p);
        if(command == ""){
            return ;
        }
        string cmd = splitCommand(command);
        //ʣ�µ���ָ���еĲ���
        cout << endl << command << endl;
        command = command.substr(cmd.length() + 1,command.length());
        switch(this->commandMap[cmd]){
            case 0:
                writeMem(command);
                break;
            case 1:
                accessMem(command);
                break;
            case 2:
                useCPU(command);
                break;
            case 3:
                useIO(command);
                break;
            default:
                return ;
        }
    }
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
            // delete *it;
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
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        cout << (*it)->id << " " << endl;
    }
    return ;
}



/*** 
 * @brief get selected process information in the fcfs queue
 * @param {int} pid
 * @return ָ��Ŀ���ָ��
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
    commandMap["IO"] = 3;
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
        Sleep(time * 100);
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
void ProcManagerFCFS::accessMem(string command){
    int addr = atoi(command.c_str());
    cout << "access Memory at addr" << " " << addr << endl;
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
void ProcManagerFCFS::writeMem(string command){
    int pos = command.find(' ');
    string addr = command.substr(0,pos);
    string num = command.substr(pos + 1,command.length());
    int number = atoi(num.c_str());
    int address = atoi(addr.c_str());
    cout << "write Memory at addr" << " " << addr << " with number " << number << endl;
    return ;
}

ProcManagerFCFS::ProcManagerFCFS(){
    initCmdMap();
}



/*****************************************************************************************
 * ProcManager����йض��岿��
 ****************************************************************************************/

/**
 * @brief Construct a new Proc Manager:: Proc Manager object
 * ProcManager�Ĺ��캯��
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
    srand(time(NULL));
    // ���ᱻ������PCB
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
    // ��������
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

    // ��rr����pcb��֮�������
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
    return rr_queue->getSize() + fcfsProcManager->getQueueSize();
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
    is_found = this->rr_queue->removePCB(pid) || this->fcfsProcManager->removeProc(pid);
    if (is_found)
    {
        printf("[%ld]Active pid=%d is killed.\n", clock() - system_start, pid);
        this->maintain();
        return;
    }

    // �ڵȴ���������Ѱ�ҽ���
    for (auto it=this->waiting_pcb.begin(); it < this->waiting_pcb.end(); it++)
    {
        // �ҵ��ˣ�ɾ����
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
    printf("[%ld]Can't find pid=%d.\n", clock() - system_start, pid);
}

/**
 * �г���ǰ���д��ڵ�pcb
 */
void ProcManager::ps()
{  
    cout << "Waiting:" << this->waiting_pcb.size() << endl;
    for (PCB* pcb : waiting_pcb)
    {
        cout << "pid: " << pcb->id << " name:" << pcb->name ;
        cout << " pri: " << pcb->pri << " need: " << pcb->time_need << endl;
    }
    cout << "Running: " << this->getActiveNum() << endl;
    cout << "RR: " << this->rr_queue->getSize() << endl;
    cout << "FCFS: " << this->fcfsProcManager->getQueueSize() << endl;
    rr_queue->getInfo();
    fcfsProcManager->getFcfsInfo();
}

/**
 * �г�ָ����pcb
 * @param pid ָ��
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
        }
    }
    if (target != nullptr)
    {
        cout << "pid: " << target->id << " name:" << target->name ;
        cout << " pri: " << target->pri << " need: " << target->time_need << endl;
    }
    else
    {
        cout << "Can't find pid " << pid << endl;
    }
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
    this->cpid = (this->cpid + 1) % 65536;
    new_pcb->name = file_name;
    new_pcb->status = NEW;
    new_pcb->pri = HIGH_PRI;
    new_pcb->time_need = 1888;
    new_pcb->slice_cnt = 0;
    PCB* pcb = new_pcb;

    // �ж��Ƿ���Ҫ���뵽�ȴ�����
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
 * ��ʼ����
 */
void ProcManager::scheduling()
{
    rr_queue->scheduling(fcfsProcManager);
    fcfsProcManager->runProcManager();
}

/**
 * @brief �ͷ�pcbռ�õĿռ�
 * 
 * @param target ָ���ͷ�pcb��ָ��
 * @return true �ɹ�
 * @return false ʧ��
 */
bool ProcManager::freePCB(PCB* target)
{
    // TODO ��������Ҫ�����ͷ�������Դ�Ĳ���
    delete target;
    return true;
}

/**
 * @brief ά�����ȶ���
 * 
 */
void ProcManager::maintain()
{
    // ά��RR����
    if (rr_queue->getSize() < MAX_PROC && waiting_pcb.size() > 0)
    {
        for (auto it = waiting_pcb.begin(); it != waiting_pcb.end(); it++)
        {
            PCB* new_pcb = *it;
            if (new_pcb->pri == HIGH_PRI)
            {
                waiting_pcb.erase(it);
                rr_queue->addPCB(new_pcb);
                printf("[%ld]Pid %d is running.\n", clock() - system_start, new_pcb->id);
                break;
            }
        }
    }
    // ά��fcfs����
    if (fcfsProcManager->getQueueSize() < MAX_PROC && waiting_pcb.size() > 0)
    {
        for (auto it = waiting_pcb.begin(); it != waiting_pcb.end(); it++)
        {
            PCB* new_pcb = *it;
            if (new_pcb->pri == LOW_PRI)
            {
                waiting_pcb.erase(it);
                fcfsProcManager->addToQueue(new_pcb);
                printf("[%ld]Pid %d is running.\n", clock() - system_start, new_pcb->id);
                break;
            }
        }
    }
}

/**
 * @brief ���Է��ص����ĺ���
 * 
 * @return ProcManager& 
 */
ProcManager& ProcManager::getInstance() 
{
    // ������
    static ProcManager instance(5,3);
    // static ProcManager instance;
    return instance;
}

// int main()
// {
//     // RRQueue test_queue(5,2);
//     // ϵͳ��ʼ��ʱ��ʵ��Ӧ�ø���
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