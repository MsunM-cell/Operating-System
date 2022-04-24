#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <cstdio>
#include <map>
#include <math.h>
#include <string.h>
#include <unordered_map>
#include "../../FileManager/json.hpp" 
#include <fstream> 
#include "../../lib/sys.h"
using namespace std;
using nlohmann::json;

typedef pair<int, int> PII;

//内存配置，从配置文件读取
struct memory_configuration {

    int PAGE_SIZE; //一页的大小
    int FRAME_NUM; //帧的数量
    int SWAP_MEMORY_SIZE; //交换区大小
    bool IS_VIRTUAL; //是否开启虚拟内存
    const int MEM_SIZE = 4 * 1024 * 1024;//物理内存大小（固定）
    int BLOCK_ALGORITHM;//动态分区算法
}mem_config;

class MemoryManager
{
public:
    char* memory;//内存数组，动态申请4MB

    //初始配置文件
    MemoryManager();
    ~MemoryManager();
    virtual int createProcess(PCB& p) = 0;
    virtual int freeProcess(PCB& p) = 0;
    virtual char accessMemory(int pid, int address) = 0;
    virtual int writeMemory(int logicalAddress, long long src, int size, unsigned int pid) = 0;

};

MemoryManager::MemoryManager()
{
    json cfgFile;
    ifstream in("git_main/Operating-System/MemoryManager/Manager/cfg", ios::binary);
    if (!in.is_open())
    {
        cout << "Error opening file\n";
        exit(1);
    }
    in >> cfgFile;
    //读取子节点信息
    mem_config.PAGE_SIZE = cfgFile["content"]["Page_size"];
    mem_config.FRAME_NUM = mem_config.MEM_SIZE / mem_config.PAGE_SIZE;
    string blockAlgorithm = cfgFile["content"]["Block_algorithm"];
    mem_config.BLOCK_ALGORITHM = (blockAlgorithm == "BF");
    string isVirtual = cfgFile["content"]["Virtual_memory"];
    mem_config.IS_VIRTUAL = (isVirtual == "yes");
    mem_config.SWAP_MEMORY_SIZE = cfgFile["content"]["Swap_memory_size"];

    cout << "Configuration Complete!\n"
         << endl;
    in.close();

    memory = new char[mem_config.MEM_SIZE]{};
}

MemoryManager::~MemoryManager()
{
    delete[] memory;
    cout << "Exit Memory Manager!\n\n";
}

