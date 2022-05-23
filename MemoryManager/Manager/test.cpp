#include "MemoryManager.cpp"


int main()
{
    PCB a, b, c, d;
    a.id = 1001, a.size = 12 * 1024, a.path = "home/test"; // 12KB
    b.id = 1002, b.size = 6 * 1024, b.path = "home/test";  // 6KB
    c.id = 1003, c.size = 10 * 1024, c.path = "home/test"; // 10KB
    d.id = 1004, d.size = 4 * 1024, d.path = "home/test";  // 4KB

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
    bmm->dms_command();
    bmm->accessMemory(a.id, 200);
    char s = 'a';
    bmm->writeMemory(100, s, a.id);
    bmm->accessMemory(a.id, 200);
    return 0;
}