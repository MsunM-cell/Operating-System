/*
@Author:Yuxing Long
@Date:2022.4.8
@Content:The basic page allocation
*/

#include "memory.h"
#include "../../lib/sys.h"

//初始化基本分页管理系统
void BasicPageManager::init_manager()
{
  //读取配置文件，初始化页面大小和数量
}

//为进程分配内存，注意页表也放在内存中
void BasicPageManager::allocate_proc_mem(const PCB &p)
{
  // vector<int>& thispagetable=pagetable[p.id];
  int size = p.size;
  int pagenum = ceil((double)size / page_size);                //进程占用页数
  int pagetablelength = ceil((double)pagenum * 4 / page_size); // 1个页表项4B
  //总空闲页数cnt和连续空闲页数succ
  int cnt = 0, succ = 0;
  for (int i = 0, j = 0; i < pagenum; i++)
    if (!((bitmap >> i) & 1))
      cnt++, j++;
    else
    {
      succ = max(succ, j);
      j = 0;
    }

  //如果进程+页表内存不够或者页表长度超过最大连续空闲页，分配失败
  if (cnt < pagenum + pagetablelength || succ < pagetablelength)
    puts("This process is too large to load in the memory!");
  else
  {
    pagetable &thispagetable = pagetablemanager[p.id];
    int i = 0;                                //帧号
    for (int j = 0; j < pagetablelength; i++) //
      if (!((bitmap >> i) & 1))
      {
        bitmap |= 1 << i;
        if (!j)
          thispagetable.start_page = i; //首页
        j++;
      }
    for (; thispagetable.size() < pagenum; i++)
      if (!((bitmap >> i) & 1))
      {
        bitmap |= 1 << i;
        thispagetable.table.push_back(i);
      }
    thispagetable.length = pagenum;       //页表table长度，即页数
    thispagetable.size = pagetablelength; //占用页数
  }
}

//释放内存
void BasicPageManager::deallocate_proc_mem(const PCB &p)
{
  if (pagetablemanager.find(p.id) == pagetablemanager.end())
    printf("Process %d is not in memory,can not deallocate...\n", p.id);
  else
  {
    pagetable &temp = pagetablemanager[p.id];
    //消除页表占用页
    for (int i = 0; i < temp.size; i++)
      bitmap ^= 1 << (temp.start_page + i);
    //消除进程占用页
    for (int i = 0; i < temp.table.size(); i++)
      bitmap ^= 1 << temp.table[i];
    //消除系统页表记录
    pagetablemanager.erase(p.id);
  }
}

//打印帧使用情况
void BasicPageManager::print_frame()
{
  for (int i = 0; i < total_page_num; i++)
    printf("%d ", (bitmap >> i) & 1); // 1表示使用，0表示未使用
  printf("\n");
}

//打印进程页表
void BasicPageManager::print_pagetable(int pid)
{
  if (pagetablemanager.find(pid) == pagetablemanager.end())
    printf("Process %d is not in memory,can not print...\n", pid);
  else
  {
    pagetable &temp = pagetablemanager[pid];
    printf("Here's pagetable of process %d\n" pid);
    for (int i = 0; i < temp.table.size(); i++)
      printf("%d : %d\n", i, temp.table[i]);
    printf("The pagetable starts in page %d, length %d\n\n", temp.start_page, temp.size);
  }
}
