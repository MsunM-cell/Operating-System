/*
 * @Date: 2022-04-08 15:46:36
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 16:56:37
 * @FilePath: \Operating-System\MemoryManager\Manager\MyFileManager.cpp
 */
#include "../Interface/FileManager.cpp"
#include <vector>
#include <map>
#include <string>


class MyFileManager : public FileManager
{
private:
    std::map<int, char *> myMap;
    int addressCount;

public:
    static MyFileManager *instance;
    static MyFileManager *getInstance();
    char *readData(unsigned long address, unsigned int length);
    unsigned long write(char *src, unsigned int length);
    MyFileManager(/* args */);
    ~MyFileManager();
};

MyFileManager::MyFileManager(/* args */)
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
        instance = new MyFileManager();
        return instance;
    }
}

char *MyFileManager::readData(unsigned long address, unsigned int length)
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

unsigned long MyFileManager::write(char *src, unsigned int length)
{
    char *temp = new char[length];
    memcpy(temp, src, length);
    myMap[addressCount] = temp;
    addressCount++;
    return addressCount;
}