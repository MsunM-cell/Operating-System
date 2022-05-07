/*
 * @Date: 2022-04-28 19:21:35
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 23:15:09
 * @FilePath: \Operating-System\MemoryManager\Manager\mem.h
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */
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
struct memory_configuration
{

    int PAGE_SIZE;                        //一页的大小
    int FRAME_NUM;                        //帧的数量
    int SWAP_MEMORY_SIZE;                 //交换区大小
    bool IS_VIRTUAL;                      //是否开启虚拟内存
    const int MEM_SIZE = 4 * 1024 * 1024; //物理内存大小（固定）
    int BLOCK_ALGORITHM;                  //动态分区算法
    int MANAGER_TYPE;
} mem_config;

class MemoryManager
{
private:
    const static int BASIC_PAGE_MEMORY_MANAGER = 0;
    const static int VIRTUAL_PAGE_MAMORY_MANAGER = 1;
    const static int BLOCK_MEMORY_MANAGER = 2;
    
public:
    char *memory; //内存数组，动态申请4MB
    static MemoryManager *instance;
    static MemoryManager *getInstance();
    static void init_config();
    //初始配置文件
    MemoryManager();
    virtual ~MemoryManager();
    virtual int createProcess(PCB &p);
    virtual int freeProcess(PCB &p);
    virtual char accessMemory(int pid, int address);
    virtual int writeMemory(int logicalAddress, const char *src, int size, unsigned int pid);
};
