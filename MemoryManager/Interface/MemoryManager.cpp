/*
 * @Date: 2022-03-24 13:33:24
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 14:52:32
 * @FilePath: \Operating-System\MemoryManager\Interface\MemoryManager.cpp
 */
class MemoryManager
{

public:
    virtual int memoryAlloc(int pid, int length) = 0;
    virtual bool memoryFree(int pid, int address, int length) = 0;
    virtual bool freeAll(int pid) = 0;
    virtual char accessMemory(int pid, int address) = 0; //¶ÁÒ»¸ö×Ö½Ú£¿
};