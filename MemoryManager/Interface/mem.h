/*
 * @Date: 2022-04-22 16:13:11
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-22 18:57:14
 * @FilePath: \Operating-System\MemoryManager\Interface\mem.h
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
#include "../../FileManager/json.hpp"
#include <string.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "../../lib/sys.h"
using namespace std;
using nlohmann::json;

//�ڴ����ã��������ļ���ȡ
struct memory_configuration
{

    int PAGE_SIZE;                        //һҳ�Ĵ�С
    int FRAME_NUM;                        //֡������
    int SWAP_MEMORY_SIZE;                 //��������С
    bool IS_VIRTUAL;                      //�Ƿ��������ڴ�
    const int MEM_SIZE = 4 * 1024 * 1024; //�����ڴ��С���̶���
    int BLOCK_ALGORITHM;                  //��̬�����㷨
} mem_config;

class MemoryManager
{
public:
    //��ʼ�����ļ�
    MemoryManager();
    ~MemoryManager();
    virtual int createProcess(PCB &p) = 0;
    virtual int freeProcess(PCB &p) = 0;
    virtual char accessMemory(int pid, long long address) = 0;
    virtual int writeMemory(long long logicalAddress, const void *src, long long size, unsigned int pid) = 0;
    char *memory; //�ڴ����飬��̬����4MB
private:
};

MemoryManager::MemoryManager()
{
    //��ȡ�����ļ�����ʼ���ڴ�����
    json cfgFile;
    ifstream in("config.json", ios::in);
    if (!in.is_open())
    {
        cout << "Error opening file\n";
        exit(1);
    }
    in >> cfgFile;
    //��ȡ�ӽڵ���Ϣ
    mem_config.PAGE_SIZE = (int)cfgFile["content"]["Page_size"];
    mem_config.FRAME_NUM = mem_config.MEM_SIZE / mem_config.PAGE_SIZE;
    string blockAlgorithm = (string)cfgFile["content"]["Block_algorithm"];
    mem_config.BLOCK_ALGORITHM = (blockAlgorithm == "BF");
    string isVirtual = (string)cfgFile["content"]["Virtual_memory"];
    mem_config.IS_VIRTUAL = (isVirtual == "yes");
    mem_config.SWAP_MEMORY_SIZE = (int)cfgFile["content"]["Swap_memory_size"];

    cout << "Configuration Complete!\n"
         << endl;
    in.close();

    memory = new char[mem_config.MEM_SIZE];
}

MemoryManager::~MemoryManager()
{
    delete[] memory;
    cout << "Exit Memory Manager!\n\n";
}
