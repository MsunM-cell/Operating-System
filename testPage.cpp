#include "./MemoryManager/Manager/MemoryManager.cpp"

int main()
{

    PCB p;
    p.id = 1001;
    p.size = 1024;
    PageMemoryManager *memoryManager = PageMemoryManager::getInstance();
    memoryManager->createProcess(p);
    cout << memoryManager->accessMemory(p.id, 0) << endl;
    cout << memoryManager->accessMemory(p.id, 1) << endl;
    cout << memoryManager->accessMemory(p.id, 2) << endl;
    cout << memoryManager->accessMemory(p.id, 3) << endl;
    cout << memoryManager->accessMemory(p.id, 4) << endl;
    cout << memoryManager->accessMemory(p.id, 5) << endl;
    memoryManager->freeProcess(p);
}
