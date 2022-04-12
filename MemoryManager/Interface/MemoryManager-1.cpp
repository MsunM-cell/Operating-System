/*
 * @Date: 2022-03-24 19:43:00
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-10 10:44:35
 * @FilePath: \Operating-System\MemoryManager\Interface\MemoryManager-1.cpp
 */

class MemoryManager
{

public:
    virtual int memoryAlloc(unsigned int pid, long long length) = 0;
    virtual bool memoryFree(unsigned int pid, long long address, long long length) = 0;
    virtual bool freeAll(unsigned int pid) = 0;
    virtual char accessMemory(unsigned int pid, long long address) = 0; 
};