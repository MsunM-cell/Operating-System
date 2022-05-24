/*
 * @Date: 2022-04-08 15:46:36
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-23 19:24:18
 * @FilePath: \Operating-System\MemoryManager\Entity\MyFileManager.cpp
 */

#include <map>
#include <string>

class MyFileManager
{
private:
    std::map<int, char *> myMap;
    int addressCount;

public:
    static MyFileManager *instance;
    static MyFileManager *getInstance();
    char *readData(long long address, unsigned int length);
    long long write(char *src, unsigned int length);
    MyFileManager(int block_size, int track_num, int sector_num);
    ~MyFileManager();
};

MyFileManager *MyFileManager::instance = nullptr;

MyFileManager::MyFileManager(int block_size, int track_num, int sector_num)
{
    addressCount = 0;
}

MyFileManager::~MyFileManager()
{
}

MyFileManager *MyFileManager::getInstance()
{
    if (instance)
    {
        return instance;
    }
    else
    {
        instance = new MyFileManager(512,200,12);
        return instance;
    }
}

char *MyFileManager::readData(long long address, unsigned int length)
{
    auto iter = myMap.find(address);
    if (iter == myMap.end())
    {
        return NULL;
    }
    else
    {
        char *temp = iter->second;
        char *result = new char[length];
        memcpy(result, temp, length);
        myMap.erase(iter);
        delete temp;
        return result;
    }
}

long long MyFileManager::write(char *src, unsigned int length)
{
    char *temp = new char[length];
    memcpy(temp, src, length);
    myMap[addressCount] = temp;
    addressCount++;
    return addressCount-1;
}