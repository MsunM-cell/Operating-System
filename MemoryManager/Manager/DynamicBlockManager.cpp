/*
@Author:Yuxing Long
@Date:2022.4.3
@Content:The dynamic block allocation(First fit & Best fit)
*/

#include "memory.h"
#include "../lib/sys.h"

//空闲分区链表排序,type=0为地址递增排序，type=1为容量递增排序
void BlockMemoryManager::adjust_list(int type)
{
	vector<PII> temp;
	free_list p = free_block_list->next;
	while (p)
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
	while (p)
	{
		p->head_addr = temp[i].first;
		p->len = temp[i++].second;
		p = p->next;
	}
}

//搜索满足条件的空闲块，并返回空闲块首地址
void BlockMemoryManager::search_free_block(const PCB& p)
{
	free_list q = free_block_list->next;
	while (q && q->len < p.size)
		q = q->next;
	if (q == NULL)
		printf("There are no more memory for this process(%d)!\n\n", p.id);
	else
	{
		int addr = q->head_addr;
		pid2addr[p.id] = addr;
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
		printf("Process %d starts from %d, length %d\n\n", p.id, addr, p.size);
		adjust_list(adjust_mode);
	}
}

//释放内存，传入参数pcb
void BlockMemoryManager::deallocate_process_mem(const PCB& p)
{
	int addr = pid2addr[p.id];
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
	//删除记录
	pid2addr.erase(p.id);
	//释放完重新调整链表
	adjust_list(adjust_mode);
	//adjust_list(1);
	printf("Free process(%d) block...\nMemory %d to %d is deallocated...\n\n", p.id, addr, addr + length - 1);
}

//初始化内存管理系统
void BlockMemoryManager::init_manager()
{
	//读取配置文件，尤其是动态分区的分配策略
	//调用init_list
	init_list();
}

//初始化空闲分区链表
void BlockMemoryManager::init_list()
{
	free_block_list = (free_list)malloc(sizeof(free_block));
	free_list q = (free_list)malloc(sizeof(free_block));
	q->head_addr = 0;
	q->len = memory_size;
	q->next = NULL;
	free_block_list->next = q;
}

//打印空闲分区链
void BlockMemoryManager::print_list()
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
	PCB a, b, c, d;
	a.id = 1001, a.size = 12 * 1024;//12KB
	b.id = 1002, b.size = 6 * 1024;//6KB
	c.id = 1003, c.size = 10 * 1024;//10KB
	d.id = 1004, d.size = 4 * 1024;//4KB
	BlockMemoryManager bmm;
	bmm.print_list();
	bmm.search_free_block(a);
	bmm.search_free_block(b);
	bmm.search_free_block(c);
	bmm.print_list();
	bmm.deallocate_process_mem(b);
	bmm.search_free_block(d);
	bmm.print_list();
	bmm.deallocate_process_mem(a);
	bmm.search_free_block(b);
	bmm.print_list();
	bmm.deallocate_process_mem(d);
	bmm.print_list();
	bmm.deallocate_process_mem(b);
	bmm.print_list();
	bmm.deallocate_process_mem(c);
	bmm.print_list();
	return 0;
}
