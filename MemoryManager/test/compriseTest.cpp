#include "../Manager/MemoryManager.cpp"

int main()
{
    MemoryManager *manager = MemoryManager::getInstance();
    // PageMemoryManager* manager = new PageMemoryManager();
    PCB *new_pcb = new PCB;
    new_pcb->id = 0;
    new_pcb->size = 1024;
    manager->createProcess((*new_pcb));
    manager->accessMemory(1, 1234567);
    manager->freeProcess(*new_pcb);
    delete manager;
}