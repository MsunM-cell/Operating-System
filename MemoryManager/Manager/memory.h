#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>
#include <cstdio>
#include <map>
#include "../../lib/sys.h"
using namespace std;

typedef pair<int, int> PII;

//内存大小
const int memory_size = 32 * 1024;

//空闲分区链的node
typedef struct free_block_node
{
  int head_addr; //首地址
  int len;       //长度
  free_block_node *next;
} free_block, *free_list;

typedef struct pagetable
{
  int start_page;//起始页表号
  int length;//table表项数量
  int size;//占用页表数量
  vector<int> table;//页表内容
}pagetable;

//容量递增排序方法
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//地址递增排序方法
bool cmp2(PII x, PII y) { return x.first < y.first; }

class MemoryManager
{
public:
  virtual void adjust_list(int type) = 0;
  virtual void search_free_block(const PCB &p) = 0;
  virtual void deallocate_process_mem(const PCB &p) = 0;
  virtual void init_list() = 0;
  virtual void print_list() = 0;
};

class BlockMemoryManager : public MemoryManager
{
private:
  //空闲分区链表
  free_list free_block_list;
  //进程和首地址对应表
  map<int, int> pid2addr;
  //当前分配策略，0为首次适应，1为最佳适应
  int adjust_mode = 0;
  //读取配置文件，修改分配策略
  void modify_tactic(int new_mode) { adjust_mode = new_mode; }
  //初始化该内存管理系统
  void init_manager();

public:
  //构造函数
  BlockMemoryManager() { init_manager(); }
  //析构函数
  ~BlockMemoryManager() { printf("Exit the  dynamic block system\n"); }
  //空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
  void adjust_list(int type);
  //搜索满足条件的空闲块，并返回空闲块首地址
  void search_free_block(const PCB &p);
  //释放内存，传入参数pcb
  void deallocate_process_mem(const PCB &p);
  //初始化空闲分区链表
  void init_list();
  //打印空闲分区链
  void print_list();
};

class BasicPageManager : public MemoryManager
{
private:
  //所有程序页表:pid->页表
  map<int, pagetable> pagetablemanager;
  //帧的使用情况
  unsigned int bitmap;
  //页大小，1-4KB之间
  int page_size = 1024;
  //总页数
  int total_page_num = memory_size / page_size;
  //修改页大小
  void modify_page_size(int new_size)
  {
    page_size = new_size;
    total_page_num = memory_size / page_size;
  }
  //初始化基本分页系统
  void init_manager();

public:
  //构造函数和析构函数
  BasicPageManager() { init_manager(); }
  ~BasicPageManager() { puts("Exit the basic page system"); }
  //分配进程内存
  void allocate_proc_mem(const PCB &p);
  //释放内存
  void deallocate_proc_mem(const PCB &p);
  //打印帧使用情况
  void print_frame();
  //打印进程页表
  void print_pagetable(int pid);
}
