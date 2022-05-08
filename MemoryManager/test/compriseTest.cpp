#include "../Manager/MemoryManager.cpp"

int main()
{
    PageMemoryManager* manager = new PageMemoryManager();
    PCB *new_pcb = new PCB;
    manager->createProcess((*new_pcb));
    manager->accessMemory(1,1234567);
    manager->freeProcess(*new_pcb);
    delete manager;
}