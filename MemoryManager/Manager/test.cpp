#include "MemoryManager.cpp"

int main()
{
    PCB a, b, c, d;
    a.id = 1001, a.size = 1234, a.path = "MemoryManager/Manager/test"; // 12KB
    b.id = 1002, b.size = 2 * 1024, b.path = "MemoryManager/Manager/test";  // 6KB
    c.id = 1003, c.size = 500, c.path = "MemoryManager/Manager/test"; // 10KB
    d.id = 1004, d.size = 2 * 1024+1, d.path = "MemoryManager/Manager/test";  // 4KB

    MemoryManager *bmm = MemoryManager::getInstance();
    bmm->createProcess(a);
    bmm->dms_command();
    bmm->createProcess(b);
    bmm->dms_command();
    bmm->createProcess(c);
    bmm->dms_command();
    bmm->freeProcess(b);
    bmm->dms_command();
    bmm->createProcess(d);
    bmm->dms_command();
    bmm->compress_mem();
    //bmm->dms_command();
    printf("pid[%d]:%c in position 200\n\n", a.id, bmm->accessMemory(a.id, 200));
    // bmm->accessMemory(a.id, 200);
    bmm->writeMemory(200, 'a', a.id);
    printf("pid[%d]:%c in position 200\n\n", a.id, bmm->accessMemory(a.id, 200));
    //bmm->accessMemory(a.id, 200);
    return 0;
}