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

//�ڴ����ã��������ļ���ȡ
struct memory_configuration
{

    int PAGE_SIZE;                        //һҳ�Ĵ�С
    int FRAME_NUM;                        //֡������
    int SWAP_MEMORY_SIZE;                 //��������С
    bool IS_VIRTUAL;                      //�Ƿ��������ڴ�
    const int MEM_SIZE = 4 * 1024 * 1024; //�����ڴ��С���̶���
    int BLOCK_ALGORITHM;                  //��̬�����㷨
    int MANAGER_TYPE;
} mem_config;

class MemoryManager
{
private:
    const static int BASIC_PAGE_MEMORY_MANAGER = 0;
    const static int VIRTUAL_PAGE_MAMORY_MANAGER = 1;
    const static int BLOCK_MEMORY_MANAGER = 2;
    
public:
    char *memory; //�ڴ����飬��̬����4MB
    static MemoryManager *instance;
    static MemoryManager *getInstance();
    static void init_config();
    //��ʼ�����ļ�
    MemoryManager();
    virtual ~MemoryManager();
    virtual int createProcess(PCB &p);
    virtual int freeProcess(PCB &p);
    virtual char accessMemory(int pid, int address);
    virtual int writeMemory(int logicalAddress, const char *src, int size, unsigned int pid);
};
