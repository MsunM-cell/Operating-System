/*
 * @Date: 2022-03-24 19:43:00
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 15:29:44
 * @FilePath: \Operating-System\MemoryManager\Interface\MemoryManager-1.cpp
 */

class MemoryManager
{

public:
    virtual int memoryAlloc(unsigned int pid, unsigned long length) = 0;
    virtual bool memoryFree(unsigned int pid, unsigned long address, unsigned long length) = 0;
    virtual bool freeAll(unsigned int pid) = 0;
    virtual char accessMemory(unsigned int pid, unsigned long address) = 0; 
};