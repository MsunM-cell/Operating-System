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

//????????????
typedef struct memory_configuration
{

    int PAGE_SIZE;                        //?????
    int FRAME_NUM;                        //????
    int SWAP_MEMORY_SIZE;                 //?????
    bool IS_VIRTUAL;                      //????????
    const int MEM_SIZE = 4 * 1024 * 1024; //??????????
    int BLOCK_ALGORITHM;                  //??????
    bool IS_PAGE;                         //??????
} configuration;
configuration mem_config, mem_config_copy;

class MemoryManager
{
public:
    char *memory; //?????????4MB

    //??????
    MemoryManager();
    ~MemoryManager();
    int createProcess(PCB &p) { return 1; }
    int freeProcess(PCB &p) { return 1; }
    char accessMemory(int pid, int address) { return '0'; }
    int writeMemory(int logicalAddress, char src, unsigned int pid) { return 1; }
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
    //???????
    mem_config.PAGE_SIZE = cfgFile["content"]["Page_size"];
    mem_config.FRAME_NUM = mem_config.MEM_SIZE / mem_config.PAGE_SIZE;
    string blockAlgorithm = cfgFile["content"]["Block_algorithm"];
    mem_config.BLOCK_ALGORITHM = (blockAlgorithm == "BF");
    string isVirtual = cfgFile["content"]["Virtual_memory"];
    mem_config.IS_VIRTUAL = (isVirtual == "yes");
    mem_config.SWAP_MEMORY_SIZE = cfgFile["content"]["Swap_memory_size"];
    string isPage = cfgFile["content"]["Page"];
    mem_config.IS_PAGE = isPage == "yes";

    cout << "Configuration Complete!\n"
         << endl;
    in.close();

    memcpy(&mem_config_copy, &mem_config, sizeof(mem_config));

    memory = new char[mem_config.MEM_SIZE]{};
}

MemoryManager::~MemoryManager()
{
    delete[] memory;
    json cfgfile;
    cfgfile["name"] = "configuration";
    cfgfile["priority"] = 1;
    cfgfile["size"] = 500;
    cfgfile["type"] = "erwx";
    cfgfile["content"]["Page_size"] = mem_config_copy.PAGE_SIZE;
    cfgfile["content"]["Page"] = (mem_config_copy.IS_PAGE ? "yes" : "no");
    cfgfile["content"]["Virtual_memory"] = (mem_config_copy.IS_VIRTUAL ? "yes" : "no");
    cfgfile["content"]["Block_algorithm"] = (mem_config_copy.BLOCK_ALGORITHM == 1 ? "BF" : "FF");
    cfgfile["content"]["Swap_memory_size"] = mem_config_copy.SWAP_MEMORY_SIZE;
    ofstream f("git_main/Operating-System/MemoryManager/Manager/cfg", ios::binary);
    f << cfgfile;
    f.close();
    cout << "Exit Memory Manager!\n\n";
}
