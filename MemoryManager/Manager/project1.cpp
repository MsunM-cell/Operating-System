#include<iostream>
#include<algorithm>
#include<vector>
#include<utility>
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
}free_block,*free_list;

//进程控制块
typedef struct pcb {
	int PID;
	int size;
	int start_addr;
}PCB;

//空闲分区链表
free_list free_block_list;
//当前分配策略，0为首次适应，1为最佳适应
int adjust_mode;

//容量递增排序方法
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//地址递增排序方法
bool cmp2(PII x, PII y) { return x.first < y.first; }

//空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
void adjust_list(int type)
{
	vector<PII> temp;
	free_list p = free_block_list->next;
	while (p != NULL)
	{
		temp.push_back({ p->head_addr,p->len });
		p = p->next;
	}
	if (type)
		sort(temp.begin(), temp.end(), cmp1);
	else
		sort(temp.begin(), temp.end(), cmp2);
	p = free_block_list->next;
	int i = 0;
	while (p != NULL)
	{
		p->head_addr = temp[i].first;
		p->len = temp[i++].second;
		p = p->next;
	}
}

//搜索满足条件的空闲块，并返回空闲块首地址
int search_free_block(PCB p)
{
	free_list q = free_block_list->next;
	while (q != NULL && q->len < p.size)
		q = q->next;
	if (q == NULL)
	{
		printf("There are no more memory for this process(%d)!\n\n", p.PID);
		return -1;
	}
	else
	{
		int addr = q->head_addr;
		q->len -= p.size;
		q->head_addr += p.size;
		if (q->len == 0)
		{
			free_list pre = free_block_list;
			//找到节点q前一个节点pre
			while (pre->next != q)
				pre = pre->next;
			pre->next = q->next;
			free(q);
		}
		printf("Process  %d starts from %d, length %d\n\n", p.PID, addr, p.size);
		adjust_list(adjust_mode);
		return addr;//返回进程开始首地址
	}
}

//释放内存，传入参数pcb
void deallocate_process_mem(PCB p)
{
	int addr = p.start_addr;
	int length = p.size;
	free_list q = free_block_list;
	adjust_list(0);//此处必须按地址排序是为了能够合并
	while (q->next && q->next->head_addr < addr)
		q = q->next;
	if (q->next && addr + length == q->next->head_addr)//和后面block合并
	{
		q->next->head_addr = addr;
		q->next->len += length;
		if (q != free_block_list && q->head_addr + q->len == addr)//和前后两个block合并
		{
			free_list temp = q->next;
			q->len += temp->len;
			q->next = temp->next;
			free(temp);
		}
	}
	else if (q != free_block_list && q->head_addr + q->len == addr)//和前面block合并
		q->len += length;
	else//新建独立block
	{
		free_list temp = (free_list)malloc(sizeof(free_block));
		temp->head_addr = addr;
		temp->len = length;
		temp->next = q->next;
		q->next = temp;
	}
	//释放完重新调整链表
	adjust_list(adjust_mode);
	//adjust_list(1);
	printf("Free process(%d) block...\nMemory %d to %d is deallocated...\n\n", p.PID, addr, addr + length - 1);
}

//初始化空闲分区链表
void init_list()
{
	free_block_list = (free_list)malloc(sizeof(free_block));
	free_list q = (free_list)malloc(sizeof(free_block));
	q->head_addr = 0;
	q->len = memory_size;
	q->next = NULL;
	free_block_list->next = q;
}

//打印空闲分区链
void print_list()
{
	printf("**********Output the free block list**********\n");
	free_list q = free_block_list->next;
	int idx = 1;
	while (q)
	{
		printf("free block %d\nstart : %d\nlength : %d\n\n", idx++, q->head_addr, q->len);
		q = q->next;
	}
	printf("**********************End*********************\n\n");
}

int main()
{
	PCB a = { 1001,12 * 1024, -1 };//20KB
	PCB b = { 1002,6 * 1024,-1 };//14KB
	PCB c = { 1003,10 * 1024,-1 };//18KB
	PCB d = { 1004,4 * 1024,-1 };//4KB
	init_list();
	print_list();
	a.start_addr = search_free_block(a);
	b.start_addr = search_free_block(b);
	c.start_addr = search_free_block(c);
	print_list();
	deallocate_process_mem(b);
	d.start_addr = search_free_block(d);
	print_list();
	deallocate_process_mem(a);
	b.start_addr = search_free_block(b);
	print_list();
	deallocate_process_mem(d);
	print_list();
	deallocate_process_mem(b);
	print_list();
	deallocate_process_mem(c);
	print_list();
	return 0;
}
