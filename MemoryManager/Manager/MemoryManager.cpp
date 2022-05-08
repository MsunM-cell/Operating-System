#include "BasicPageManager.cpp"
#include "DynamicBlockManager.cpp"
#include "PageMemoryManager.cpp"
//#include "mem.h"

MemoryManager *MemoryManager::instance = nullptr;

MemoryManager::MemoryManager()
{
    init_config();
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
    ofstream f("../Manager/cfg", ios::binary);
    f << cfgfile;
    f.close();
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
        switch (MANAGER_TYPE)
        {
        case BASIC_PAGE_MEMORY_MANAGER:
            instance = new BasicPageManager();
            break;
        case BLOCK_MEMORY_MANAGER:
            instance = new BlockMemoryManager();
            break;
        case VIRTUAL_PAGE_MAMORY_MANAGER:
            std::cout << "will create PageMemoryManager" << std::endl;

            instance = new PageMemoryManager();
            std::cout << "create PageMemoryManager" << std::endl;
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
    mem_config.IS_PAGE = (cfgFile["content"]["Page"] == "yes");
    mem_config.SWAP_MEMORY_SIZE = cfgFile["content"]["Swap_memory_size"];
    if (mem_config.IS_PAGE)
    {
        if (mem_config.IS_VIRTUAL)
            MANAGER_TYPE = VIRTUAL_PAGE_MAMORY_MANAGER;
        else
            MANAGER_TYPE = BASIC_PAGE_MEMORY_MANAGER;
    }
    else
        MANAGER_TYPE = BLOCK_MEMORY_MANAGER;
    std::cout << "Manager type" << MANAGER_TYPE << std::endl;
    cout << "Configuration Complete!\n"
         << endl;
    in.close();
}
