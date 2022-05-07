#include "../Manager/MemoryManager.cpp"

int main()
{
    MemoryManager *manager = MemoryManager::getInstance();


    PCB *new_pcb = new PCB;
    new_pcb->id = 1;
    new_pcb->name = "normal";
    new_pcb->status = READY;
    new_pcb->pri = 0;
    new_pcb->slice_cnt = 0;
    new_pcb->time_need = 0;
    new_pcb->size = 1024;

    
    manager->createProcess((*new_pcb));

    delete manager;
}