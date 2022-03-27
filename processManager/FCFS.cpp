#include <iostream>
#include "proc.h"
#include "../MemoryManager/Manager/PageMemoryManager.cpp"


/*** 
 * @description: a class to manage process with fcfs algorithm
 */
class procManagerFCFS{
    public:
        void addToQueue(PCB p);
        void runProcManager(PageMemoryManager m);
        void run(PCB p,PageMemoryManager m);
    private:
        queue <PCB> fcfsQueue;
    
};

/*** 
 * @description: push this process into the fcfs queue
 * @param {PCB} p
 * @return {*}
 */
void procManagerFCFS::addToQueue(PCB p){
    fcfsQueue.push(p);
}


/*** 
 * @description: the fcfs algorithm
 * @param {  }
 * @return {*}
 */
void procManagerFCFS::runProcManager(PageMemoryManager m){
    while(true){
        while(!fcfsQueue.empty()){
            PCB p = fcfsQueue.front();
            //该函数是执行函数，暂时未定
            run(p,m);
            fcfsQueue.pop();
        }
    }
}


/*** 
 * @description: execute the programme,not written yet
 * @param {PCB} p
 * @return {*}
 */
void procManagerFCFS::run(PCB p,PageMemoryManager m){
    // m.accessMemory(pid);
    std::cout << "process " << p.id << " is running" << endl;
}