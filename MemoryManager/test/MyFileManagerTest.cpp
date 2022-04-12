/*
 * @Date: 2022-04-09 15:31:01
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-10 10:12:09
 * @FilePath: \Operating-System\MemoryManager\test\MyFileManagerTest.cpp
 */
#include "../Manager/MyFileManager.cpp"
#include<iostream>

int main(int argc, char const *argv[])
{
    char* old = new char[10];
    for(int i = 0 ; i < 10 ; i++){
        old[i] = '0'+ i;
    }
    long long address =  MyFileManager::getInstance()->write(old,10);
    std::cout<<address<<std::endl;
    char* newChar = new char[10];
    char* data = MyFileManager::getInstance()->readData(address,10);
    if(!data){
        std::cout<<"ÔõÃ´»ØÊÂ"<<std::endl;
    }
    memcpy(newChar,data,10);
    for(int i = 0 ; i < 10 ; i++){
        std::cout<<newChar[i]<<std::endl;
    }
    return 0;
}
