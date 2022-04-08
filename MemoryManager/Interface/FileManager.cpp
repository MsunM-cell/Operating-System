/*
 * @Date: 2022-03-25 18:10:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:28:41
 * @FilePath: \Operating-System\MemoryManager\Interface\FileManager.cpp
 */

#ifndef FILE_MANAGER
#define FILE_MANAGER

class FileManager
{
private:
    /* data */
public:
    virtual char *readData(unsigned long address, unsigned int length);
    virtual unsigned long write(char *src, unsigned int length);
};

#endif