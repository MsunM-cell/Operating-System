#include<iostream>
#include<algorithm>
#include<vector>
#include<utility>
using namespace std;

typedef pair<int, int> PII;
 
//��ҳ��
const int total_page_num = 32;
//ҳ��С
const int page_size = 1024;
//�ڴ��С
const int memory_size = total_page_num * page_size;

//���з�������node
typedef struct free_block_node {
	int head_addr;//�׵�ַ
	int len;//����
	free_block_node* next;
}free_block,*free_list;

//���̿��ƿ�
typedef struct pcb {
	int PID;
	int size;
	int start_addr;
}PCB;

//���з�������
free_list free_block_list;
//��ǰ������ԣ�0Ϊ�״���Ӧ��1Ϊ�����Ӧ
int adjust_mode;

//�����������򷽷�
bool cmp1(PII x, PII y) { return x.second == y.second ? x.first < y.first : x.second < y.second; }
//��ַ�������򷽷�
bool cmp2(PII x, PII y) { return x.first < y.first; }

//���з�����������,type=0Ϊ��ַ��������type=1Ϊ������������
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

//�������������Ŀ��п飬�����ؿ��п��׵�ַ
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
			//�ҵ��ڵ�qǰһ���ڵ�pre
			while (pre->next != q)
				pre = pre->next;
			pre->next = q->next;
			free(q);
		}
		printf("Process  %d starts from %d, length %d\n\n", p.PID, addr, p.size);
		adjust_list(adjust_mode);
		return addr;//���ؽ��̿�ʼ�׵�ַ
	}
}

//�ͷ��ڴ棬�������pcb
void deallocate_process_mem(PCB p)
{
	int addr = p.start_addr;
	int length = p.size;
	free_list q = free_block_list;
	adjust_list(0);//�˴����밴��ַ������Ϊ���ܹ��ϲ�
	while (q->next && q->next->head_addr < addr)
		q = q->next;
	if (q->next && addr + length == q->next->head_addr)//�ͺ���block�ϲ�
	{
		q->next->head_addr = addr;
		q->next->len += length;
		if (q != free_block_list && q->head_addr + q->len == addr)//��ǰ������block�ϲ�
		{
			free_list temp = q->next;
			q->len += temp->len;
			q->next = temp->next;
			free(temp);
		}
	}
	else if (q != free_block_list && q->head_addr + q->len == addr)//��ǰ��block�ϲ�
		q->len += length;
	else//�½�����block
	{
		free_list temp = (free_list)malloc(sizeof(free_block));
		temp->head_addr = addr;
		temp->len = length;
		temp->next = q->next;
		q->next = temp;
	}
	//�ͷ������µ�������
	adjust_list(adjust_mode);
	//adjust_list(1);
	printf("Free process(%d) block...\nMemory %d to %d is deallocated...\n\n", p.PID, addr, addr + length - 1);
}

//��ʼ�����з�������
void init_list()
{
	free_block_list = (free_list)malloc(sizeof(free_block));
	free_list q = (free_list)malloc(sizeof(free_block));
	q->head_addr = 0;
	q->len = memory_size;
	q->next = NULL;
	free_block_list->next = q;
}

//��ӡ���з�����
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
