/*
@Author:Yuxing Long
@Date:2022.4.28
@Content:The basic page allocation
*/

#include "page.h"
#include "../../lib/sys.h"

//初始化基本分页管理系统
void BasicPageManager::init_manager()
{
  bitmap.resize(mem_config.FRAME_NUM, {-1, 0});
}

//加载进程指令
int BasicPageManager::load_ins(int pid)
{

  json root;
  ifstream in("git_main/Operating-System/MemoryManager/Manager/test", ios::binary);
  if (!in.is_open())
  {
    cout << "Error opening file\n";
    exit(1);
  }
  in >> root;
  int addr = pagetable[pid][0] * mem_config.PAGE_SIZE;
  int length = mem_config.PAGE_SIZE;
  for (int i = 0, j = 0; i < root["content"].size(); ++i)
  {
    string s = root["content"][i];
    if (length < s.size()) //帧边界处理
    {
      sprintf(memory + addr, "%s", s.substr(0, length).c_str());
      addr = pagetable[pid][++j] * mem_config.PAGE_SIZE;
      sprintf(memory + addr, "%s", s.substr(length).c_str());
      length = s.size() - length;
      addr += length;
      length = mem_config.PAGE_SIZE - length;
    }
    else
    {
      sprintf(memory + addr, "%s", s.c_str());
      length -= s.size();
      addr += s.size();
    }
  }

  in.close();
  return 1;
}

//为进程分配内存
int BasicPageManager::createProcess(PCB &p)
{
  int cnt = 0;                                                     //内存空闲帧数量
  int pagetable_len = ceil((double)p.size / mem_config.PAGE_SIZE); //进程占用帧数
  for (int i = 0; i < mem_config.FRAME_NUM; ++i)
    if (bitmap[i].first == -1)
    {
      cnt++;
      if (cnt >= pagetable_len)
        break;
    }
  //有足够页供进程分配
  if (cnt >= pagetable_len)
  {
    pagetable[p.id].resize(pagetable_len, -1);
    for (int i = 0, j = 0; j < pagetable_len; ++i)
      if (bitmap[i].first == -1)
      {
        int length = min(p.size - j * mem_config.PAGE_SIZE, mem_config.PAGE_SIZE);
        bitmap[i].first = p.id;
        bitmap[i].second = length;
        pagetable[p.id][j] = i;
        j++;
      }

    load_ins(p.id);
    return 1;
  }
  else
  {
    printf("%d : process is too large to load in memory!\n\n", p.id);
    return -1;
  }
}

//释放内存
int BasicPageManager::freeProcess(PCB &p)
{
  if (pagetable.find(p.id) == pagetable.end())
  {
    puts("%d : process not in memory\n");
    return -1;
  }
  int m = pagetable[p.id].size();
  for (int i = 0; i < m; ++i)
  {
    int pageid = pagetable[p.id][i];
    bitmap[pageid].first = -1;
    bitmap[pageid].second = 0;
  }
  pagetable.erase(p.id);
  return 1;
}

//打印帧使用情况
void BasicPageManager::print_frame()
{
  puts("**********Here's the use table of memory frame**********");
  for (int i = 0; i < 32; i++)
    printf("block #%d    %d/%d    pid:%d\n", i, bitmap[i].second, mem_config.PAGE_SIZE, bitmap[i].first); // 1表示使用，0表示未使用
  puts("\n*************************end***************************\n");
}

//打印进程页表
void BasicPageManager::print_pagetable(const PCB &p)
{
  if (pagetable.find(p.id) == pagetable.end())
  {
    printf("%d : process's pagetable is not in memory!\n\n", p.id);
  }
  else
  {
    printf("**********This is the pagetable of process %d**********\n", p.id);
    puts("logical page id : physical frame id");
    for (int i = 0; i < pagetable[p.id].size(); i++)
    {
      printf("%d : %d\n", i, pagetable[p.id][i]);
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
  bpm.createProcess(p);
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
  bpm.freeProcess(q);
  bpm.print_frame();
  test(bpm, v);
  test(bpm, q);
  bpm.freeProcess(p);
  bpm.freeProcess(u);
  bpm.freeProcess(v);
  bpm.print_frame();
  bpm.freeProcess(q);
  bpm.print_frame();

  return 0;
}
