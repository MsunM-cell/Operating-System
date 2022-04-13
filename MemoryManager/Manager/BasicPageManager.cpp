/*
@Author:Yuxing Long
@Date:2022.4.13
@Content:The basic page allocation
*/

#include "memory.h"
#include "../../lib/sys.h"

//初始化基本分页管理系统
void BasicPageManager::init_manager()
{
  //读取配置文件，初始化页面大小和数量

  bitmap = 0;
}

//分配页表内存
void BasicPageManager::create_pagetable(PCB &p)
{
  int size = p.size;
  int itemnum = ceil((double)size / page_size);        //页表项数目=程序逻辑页数
  int tablelength = ceil((double)itemnum / page_size); //页表占用页数
  int succ_idle = 0;                                   //内存最大连续空闲页数
  int s_idle = 0;                                      //最大连续空闲页数起始页
  for (int i = 0, j = 0; i <= total_page_num; ++i)
  {
    if (i < total_page_num && !((bitmap >> i) & 1))
      j++;
    else
    {
      if (j > succ_idle)
      {
        succ_idle = j;
        s_idle = i - j;
        if (succ_idle >= tablelength)
          break;
      }
      j = 0;
    }
  }
  //存在目标个数连续页
  if (succ_idle >= tablelength)
  {
    for (int i = 0; i < tablelength; ++i)
    {
      int j = i + s_idle;
      bitmap |= 1 << j; //该页标记为使用
    }
    p.pagetable_addr = s_idle * page_size;
    p.pagetable_len = itemnum;
  }
  else
  {
    printf("%d : process is too large to load in memory!\n\n", p.id);
  }
}

//释放页表内存
void BasicPageManager::delete_pagetable(PCB &p)
{
  if (p.pagetable_addr >= 0)
  {
    //页表内存清零
    memset(memory + p.pagetable_addr, 0, sizeof(char) * p.pagetable_len * PageTableItem_Byte); //
    p.pagetable_addr = -1;
    p.pagetable_len = 0;
  }
  else
  {
    printf("%d : process has no pagetable in memory!\n\n", p.id);
  }
}

//为进程分配内存，注意页表也放在内存中
void BasicPageManager::allocate_proc_mem(PCB &p)
{
  create_pagetable(p);
  int cnt = 0;
  for (int i = 0; i < total_page_num; ++i)
    if (!((bitmap >> i) & 1))
      cnt++;
  //有足够页供进程分配
  if (cnt >= p.pagetable_len)
  {
    for (int i = 0, j = 0; j < p.pagetable_len; ++i)
      if (!((bitmap >> i) & 1))
      {
        bitmap |= 1 << i;
        // snprintf(memory + p.pagetable_addr + j * PageTableItem_Byte, PageTableItem_Byte + 1, "%d", i);
        memory[p.pagetable_addr + j * PageTableItem_Byte] = i;
        // printf("%d ", memory[p.pagetable_addr + j * PageTableItem_Byte]);
        j++;
      }
  }
  else
  {
    printf("%d : process is too large to load in memory!\n\n", p.id);
    delete_pagetable(p);
  }
}

//释放内存
void BasicPageManager::deallocate_proc_mem(PCB &p)
{
  int f = p.pagetable_addr, m = p.pagetable_len;
  for (int i = 0; i < m; ++i)
  {
    int j = f + i * PageTableItem_Byte;
    char *temp_chr;
    //从页表中提取一个页表项
    // strncpy(temp_chr, memory + j, PageTableItem_Byte);
    int pageid = memory[j];
    bitmap ^= 1 << pageid; //异或和减都可以
  }
  delete_pagetable(p);
}

//打印帧使用情况
void BasicPageManager::print_frame()
{
  puts("**********Here's the use table of memory frame**********");
  for (int i = 0; i < total_page_num; i++)
    printf("%d ", (bitmap >> i) & 1); // 1表示使用，0表示未使用
  puts("\n*************************end***************************\n");
}

//打印进程页表
void BasicPageManager::print_pagetable(const PCB &p)
{
  if (p.pagetable_addr < 0)
  {
    printf("%d : process's pagetable is not in memory!\n\n", p.id);
  }
  else
  {
    printf("**********This is the pagetable of process %d**********\n", p.id);
    puts("逻辑页号 : 物理帧号");
    for (int i = 0; i < p.pagetable_len; i++)
    {
      int j = p.pagetable_addr + i * PageTableItem_Byte;
      // char temp_chr[PageTableItem_Byte];
      //从页表中提取一个页表项
      // strncpy(temp_chr, memory + j, PageTableItem_Byte);
      printf("%d : %d\n", i, memory[j]);
    }
    puts("***************************end**************************\n");
  }
}

void init_pcb(int id, int size, PCB &p)
{
  p.id = id;
  p.pagetable_addr = -1;
  p.pagetable_len = 0;
  p.size = size;
}

void test(BasicPageManager &bpm, PCB &p)
{
  bpm.allocate_proc_mem(p);
  bpm.print_frame();
  bpm.print_pagetable(p);
}

int main()
{
  //测试进程
  PCB p, q, u, v;
  init_pcb(001, 12345, p); // 13页
  init_pcb(102, 12900, q); // 13页
  init_pcb(666, 2049, u);  // 3页
  init_pcb(321, 6000, v);  // 6页

  BasicPageManager bpm;
  test(bpm, p);
  test(bpm, q);
  test(bpm, u);
  bpm.deallocate_proc_mem(q);
  test(bpm, v);
  test(bpm, q);

  return 0;
}
