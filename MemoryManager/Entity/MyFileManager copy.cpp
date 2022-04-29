/*
 * @Date: 2022-04-29 09:44:20
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-29 09:44:21
 * @FilePath: \Operating-System\MemoryManager\Entity\MyFileManager copy.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
/*
 * @Date: 2022-04-08 15:46:36
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-28 20:53:21
 * @FilePath: \Operating-System\MemoryManager\Entity\MyFileManager.cpp
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
    bool readData(long long address, unsigned int length,std::string file);
    bool write(char *src, unsigned int length,std::string file);
    MyFileManager(/* args */);
    ~MyFileManager();
};

MyFileManager *MyFileManager::instance = nullptr;

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