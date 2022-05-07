/*
@Author:Yuxing Long
@Date:2022.4.22
@Content:The dynamic block allocation(First fit & Best fit)
@Adding configuration file reading
*/

#include "block.h"

//空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
void BlockMemoryManager::adjust_list(int type)
{
    if (type)
        sort(free_block_table.begin(), free_block_table.end(), cmp1);
    else
        sort(free_block_table.begin(), free_block_table.end(), cmp2);
}

//加载指令
int BlockMemoryManager::load_ins(int addr, int length)
{

    json root;
    ifstream in("../MemoryManager/Manager/test", ios::binary);
    if (!in.is_open())
    {
        cout << "Error opening file\n";
        exit(1);
    }
    in >> root;
    for (int i = 0, j = addr; i < root["content"].size(); ++i)
    {
        string s = root["content"][i];
        s += '\0';
        //s = s.substr(1, s.size() - 2);
        sprintf(memory + j, "%s", s.c_str());
        j += s.size();
    }

    in.close();
    return 1;
}

//搜索满足条件的空闲块，并返回空闲块首地址
int BlockMemoryManager::createProcess(PCB &p)
{
    int i = 0;
    while (i < free_block_table.size() && free_block_table[i].len < p.size)
        i++;

    if (i == free_block_table.size())
    {
        printf("There are no more memory for this process(%d)!\n\n", p.id);
        return -1;
    }
    else
    {
        int addr = free_block_table[i].head_addr;
        pid2addr[p.id].head_addr = addr;
        pid2addr[p.id].len = p.size;
        addr2pid[addr] = p.id;
        free_block_table[i].len -= p.size;
        free_block_table[i].head_addr += p.size;
        if (free_block_table[i].len == 0)
        {
            free_block_table.erase(free_block_table.begin() + i);
        }
        printf("Process %d starts from %d, length %d\n\n", p.id, addr, p.size);
        adjust_list(mem_config.BLOCK_ALGORITHM);

        //加载指令
        load_ins(addr, p.size);
        return 1;
    }
}

//释放内存，传入参数pcb
int BlockMemoryManager::freeProcess(PCB &p)
{
    int addr = pid2addr[p.id].head_addr;
    int length = p.size;

    adjust_list(0); //此处必须按地址排序是为了能够合??
    int i = 0;
    while (i < free_block_table.size() && free_block_table[i].head_addr < addr)
        i++;
    if (i < free_block_table.size() && addr + length == free_block_table[i].head_addr) //和后面block合并
    {
        free_block_table[i].head_addr = addr;
        free_block_table[i].len += length;
        if (i && free_block_table[i - 1].head_addr + free_block_table[i - 1].len == addr) //和前后两个block合并
        {
            free_block_table[i - 1].len += free_block_table[i].len;
            free_block_table.erase(free_block_table.begin() + i);
        }
    }
    else if (i && free_block_table[i - 1].head_addr + free_block_table[i - 1].len == addr) //和前面block合并
        free_block_table[i - 1].len += length;
    else //新建独立block
        free_block_table.insert(free_block_table.begin() + i, {addr, length});

    //删除记录
    memset(memory + addr, 0, sizeof(char) * length);
    pid2addr.erase(p.id);
    addr2pid.erase(addr);

    //释放完重新调整表
    adjust_list(mem_config.BLOCK_ALGORITHM);
    // adjust_list(1);
    printf("Free process(%d) block...\nMemory %d to %d is deallocated...\n\n", p.id, addr, addr + length - 1);
    return 1;
}

//压缩
int BlockMemoryManager::compress_mem()
{
    if (free_block_table.empty())
    {
        cout << "Memory is full,can not compress any more!\n\n";
        return 1;
    }
    int sumsize = 0; //所有进程大小和
    for (auto it = addr2pid.begin(); it != addr2pid.end(); it++)
    {
        int id = it->second;
        int length = pid2addr[id].len;
        char *s = new char[length];
        snprintf(s, length, "%s", memory + it->first);
        memset(memory + it->first, 0, sizeof(char) * length);
        snprintf(memory + sumsize, length, "%s", s);
        pid2addr[id].head_addr = sumsize;
        sumsize += length;
        delete[] s;
    }
    addr2pid.clear();
    for (auto it = pid2addr.begin(); it != pid2addr.end(); it++)
        addr2pid[it->second.first] = it->first;
    free_block_table.clear();
    free_block_table.push_back({sumsize, mem_config.MEM_SIZE - sumsize});
    return 1;
}

//访问内存
char BlockMemoryManager::accessMemory(int pid, int address)
{
    if (address < 0 || address > pid2addr[pid].len)
    {
        puts("Illegal memory access!\n\n");
        return char(-1); //访问失败返回-1
    }
    return memory[pid2addr[pid].head_addr + address];
}

//写内??
int BlockMemoryManager::writeMemory(int logicalAddress, long long src, int size, unsigned int pid)
{
    return 1;
}

//初始化内存管理系??
void BlockMemoryManager::init_manager()
{
    //初始化空闲分区表
    free_block_table.push_back({0, mem_config.MEM_SIZE});
}

//打印空闲分区??
void BlockMemoryManager::print_list()
{
    printf("**********Output the free block list**********\n");

    int idx = 1;
    for (int i = 0; i < free_block_table.size(); ++i)
        printf("free block %d\nstart : %d\nlength : %d\n\n", i, free_block_table[i].head_addr, free_block_table[i].len);
    puts("**********************End*********************\n");
}

// int main()
// {
//     PCB a, b, c, d;
//     a.id = 1001, a.size = 12 * 1024; // 12KB
//     b.id = 1002, b.size = 6 * 1024;  // 6KB
//     c.id = 1003, c.size = 10 * 1024; // 10KB
//     d.id = 1004, d.size = 4 * 1024;  // 4KB

//     BlockMemoryManager bmm;
//     bmm.print_list();
//     bmm.createProcess(a);
//     bmm.createProcess(b);
//     bmm.createProcess(c);
//     cout << bmm.accessMemory(a.id, 1) << endl;
//     bmm.print_list();
//     bmm.freeProcess(b);
//     bmm.print_list();
//     bmm.compress_mem();
//     bmm.print_list();
//     bmm.createProcess(d);
//     bmm.print_list();
//     bmm.freeProcess(a);
//     bmm.createProcess(b);
//     bmm.print_list();
//     bmm.freeProcess(d);
//     bmm.print_list();
//     bmm.freeProcess(b);
//     bmm.print_list();
//     bmm.freeProcess(c);
//     bmm.print_list();
//     return 0;
// }
