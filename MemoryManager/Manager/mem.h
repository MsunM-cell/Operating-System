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

//内存配置
typedef struct memory_configuration
{

    int PAGE_SIZE;                        //页大小
    int FRAME_NUM;                        //帧数量
    int SWAP_MEMORY_SIZE;                 //交换空间大小
    bool IS_VIRTUAL;                      //是否启动虚拟内存
    const int MEM_SIZE = 4 * 1024 * 1024; //内存大小固定
    int BLOCK_ALGORITHM;                  //动态分区算法
    bool IS_PAGE;                         //是否使用分页
} configuration;
configuration mem_config, mem_config_copy;

class MemoryManager
{
private:
    const static int BASIC_PAGE_MEMORY_MANAGER = 0;
    const static int VIRTUAL_PAGE_MAMORY_MANAGER = 1;
    const static int BLOCK_MEMORY_MANAGER = 2;
    static int MANAGER_TYPE;
    
public:
    char *memory; //物理内存4MB

    MemoryManager();
    static MemoryManager* getInstance();
    static MemoryManager* instance;
    static void init_config();
    virtual ~MemoryManager();
    virtual int createProcess(PCB &p) { return 1; }
    virtual int freeProcess(PCB &p) { return 1; }
    virtual char accessMemory(int pid, int address) { return '0'; }
    virtual int writeMemory(int logicalAddress, char src, unsigned int pid) { return 1; }
};

