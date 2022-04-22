/*
 * @Date: 2022-03-24 19:43:00
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-14 19:52:32
 * @FilePath: \Operating-System\MemoryManager\Interface\MemoryManager-1.cpp
 */

class MemoryManager
{

public:
    virtual int createProcess(unsigned int pid, long long length) = 0;
    virtual bool memoryFree(unsigned int pid, long long address, long long length) = 0;
    virtual bool freeAll(unsigned int pid) = 0;
    virtual char accessMemory(unsigned int pid, long long address) = 0;
    virtual bool write(long long logicalAddress, const void *src, long long size, unsigned int pid) = 0;
};