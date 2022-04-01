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
void ProcManagerFCFS::runProcManager(){
    while(true){
        while(!fcfsQueue.empty()){
            PCB p = fcfsQueue.front();
            //�ú�����ִ�к�������ʱδ��
            run(p);
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
    // ��Ϊ��û���ļ���ʽ��run������ʱû�а취д
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