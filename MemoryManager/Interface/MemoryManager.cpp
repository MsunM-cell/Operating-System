/*
 * @Date: 2022-03-24 13:33:24
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-03-24 13:44:57
 * @FilePath: \MemoryManager\MemoryManager.cpp
 */
class MemoryManager
{

public:
    virtual int pageAlloc(int pid, int length) = 0;
    virtual bool pageFree(int pid, int address, int length) = 0;
    virtual bool freeAll(int pid) = 0;
    virtual char accessMemory(int pid, int address) = 0; //¶ÁÒ»¸ö×Ö½Ú£¿
};