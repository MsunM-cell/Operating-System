#include "BasicPageManager.cpp"
#include "DynamicBlockManager.cpp"
#include "PageMemoryManager.cpp"
#include "mem.h"

MemoryManager *MemoryManager::instance = nullptr;

MemoryManager::MemoryManager()
{
    MemoryManager::init_config();
    //有点矛盾...如果只通过getInstance调用的话这里可以不init
    //但单独new单个类时不会调用getInstance，所以写了两遍init_config
    memory = new char[mem_config.MEM_SIZE]{};
}

MemoryManager::~MemoryManager()
{
    delete[] memory;
    cout << "Exit Memory Manager!\n\n";
}

MemoryManager *MemoryManager::getInstance()
{
    if (instance)
    {
        return instance;
    }
    else
    {
        MemoryManager::init_config();
        switch (mem_config.MANAGER_TYPE)
        {
        case BASIC_PAGE_MEMORY_MANAGER:
            instance = new BasicPageManager();
            break;
        case BLOCK_MEMORY_MANAGER:
            instance = new BlockMemoryManager();
            break;
        case VIRTUAL_PAGE_MAMORY_MANAGER:
            instance = new PageMemoryManager();
            break;
        default:
            instance = new BlockMemoryManager();
            break;
        }
        return instance;
    }
}

void MemoryManager::init_config()
{
    json cfgFile;
    ifstream in("../Manager/cfg", ios::binary);
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
    mem_config.MANAGER_TYPE = cfgFile["content"]["Manager_type"];
    std::cout << "Manager type" << mem_config.MANAGER_TYPE << std::endl;
    cout << "Configuration Complete!\n"
         << endl;
    in.close();
}