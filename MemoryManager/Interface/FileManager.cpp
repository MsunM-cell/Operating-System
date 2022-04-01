/*
 * @Date: 2022-03-25 18:10:30
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-01 19:18:22
 * @FilePath: \Operating-System\MemoryManager\Interface\FileManager.cpp
 */
class FileManager
{
private:
    /* data */
public:
    virtual char* readData(unsigned long address , unsigned int length);
    virtual unsigned long write(char* src , unsigned int length);
};
