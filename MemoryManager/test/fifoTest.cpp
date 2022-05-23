/*
 * @Date: 2022-05-23 22:32:03
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-23 22:40:30
 * @FilePath: \Operating-System\fifoTest.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
#include "./MemoryManager/Manager/MemoryManager.cpp"

int main()
{
    PCB *p = new PCB();
    p->id = 1;
    p->size = 50000;
    p->path = "/home/test3";
    MemoryManager *manager = MemoryManager::getInstance();
    cout << manager->getMode() << endl;
    manager->createProcess(*p);
    for (int i = 0; i < 50000; i = i + 1000)
    {
        manager->writeMemory(i / 8, 'h', 1);
        if (i > 20000 && i < 23000)
        {
            manager->writeMemory(0, 'h', 1);
        }
    }
}