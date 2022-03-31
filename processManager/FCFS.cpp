#include <iostream>
#include "proc.h"

/*** 
 * @description: a class to manage process with fcfs algorithm
 */
procManagerFCFS::~procManagerFCFS(){
    cout << "FCFS proc manager is terminated" << endl;
}

/*** 
 * @description: push this process into the fcfs queue
 * @param {PCB} p
 * @return {*}
 */
void procManagerFCFS::addToQueue(PCB p){
    fcfsQueue.push_back(p);
}


/*** 
 * @description: the fcfs algorithm
 * @param {  }
 * @return {*}
 */
void procManagerFCFS::runProcManager(PageMemoryManager* m){
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

string procManagerFCFS::getCommand(){

}


/*** 
 * @description: execute the programme,not written yet
 * @param {PCB} p
 * @return {*}
 */
void procManagerFCFS::run(PCB p,PageMemoryManager* m){
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
 * @description: 
 * @param {int} pid
 * @return {*}
 */
bool procManagerFCFS::removeProc(int pid){
    for(auto it = fcfsQueue.begin();it != fcfsQueue.end();it++){
        if(it->id == pid){
            it = fcfsQueue.erase(it);
            return true;
        }
    }
    cout << "no such pid" << endl;
    return false;
}