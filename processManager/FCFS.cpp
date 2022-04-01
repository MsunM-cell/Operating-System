#include <iostream>
#include "proc.h"


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
void ProcManagerFCFS::runProcManager(PageMemoryManager* m){
    while(true){
        while(!fcfsQueue.empty()){
            PCB p = fcfsQueue.front();
            //该函数是执行函数，暂时未定
            run(p,m);
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
void ProcManagerFCFS::run(PCB p,PageMemoryManager* m){
    string cpuCmd = "CPU",IOCmd = "IO",accessCmd = "access";
    int index;
    // m.accessMemory(pid);
    // 这里我们假设已经获取到了指令
    string cmd = getCommand();
    if(cmd.find(cpuCmd) != string::npos){
        index = cmd.find(cpuCmd) + 3;
        if(CPU[atoi(cmd.substr(index).c_str())]){
            CPU[atoi(cmd.substr(index).c_str())] = false;
            index = cmd.find(' ');
            string tmp = cmd.substr(index);
            Sleep(atoi(tmp.c_str()));
            CPU[atoi(cmd.substr(index).c_str())] = true;
        }
    }
    else if(cmd.find(IOCmd) != string::npos){
        index = cmd.find(IOCmd) + 2;
        if(IO[atoi(cmd.substr(index).c_str())]){
            IO[atoi(cmd.substr(index).c_str())] = false;
            string tmp = cmd.substr(index);
            index += 2;
            Sleep(atoi(tmp.c_str()));
            IO[atoi(cmd.substr(index).c_str())] = true;
        }
    }
    else if(cmd.find(accessCmd) != string::npos){
        index = cmd.find(accessCmd) + 7;
        m->accessMemory(p.id,atoi(cmd.substr(index).c_str()));
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
        if(it->id == pid){
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
        if(it->id = pid){
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