#pragma once
#include<iostream>
#include<algorithm>
#include<vector>
#include<utility>
#include<cstdio>
using namespace std;

typedef pair<int, int> PII;

//总页数
const int total_page_num = 32;
//页大小
const int page_size = 1024;
//内存大小
const int memory_size = total_page_num * page_size;

//空闲分区链的node
typedef struct free_block_node {
	int head_addr;//首地址
	int len;//长度
	free_block_node* next;
}free_block, * free_list;

//进程控制块
typedef struct pcb {
	int PID;
	int size;
	int start_addr;
}PCB;

//容量递增排序方法
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//地址递增排序方法
bool cmp2(PII x, PII y) { return x.first < y.first; }


class MemoryManager
{
public:
    //virtual int pageAlloc(int pid, int length) = 0;
    //virtual bool pageFree(int pid, int address, int length) = 0;
    //virtual bool freeAll(int pid) = 0;
    //virtual char accessMemory(int pid, int address) = 0; 

	virtual void adjust_list(int type) = 0;
	virtual int search_free_block(PCB p) = 0;
	virtual void deallocate_process_mem(PCB p) = 0;
	virtual void init_list() = 0;
	virtual void print_list() = 0;
};

class BlockMemoryManager : public MemoryManager
{
private:
	//空闲分区链表
	free_list free_block_list;
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
	~BlockMemoryManager() { printf("Exit the system\n"); }
	//空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
	void adjust_list(int type);
	//搜索满足条件的空闲块，并返回空闲块首地址
	int search_free_block(PCB p);
	//释放内存，传入参数pcb
	void deallocate_process_mem(PCB p);
	//初始化空闲分区链表
	void init_list();
	//打印空闲分区链
	void print_list();
};
