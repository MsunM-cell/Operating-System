/**
 * ϵͳ����ͷ�ļ�
 */

#ifndef SYS_H
#define SYS_H

#define SYS_NAME 114


// PCB�Ľṹ��
typedef struct PCB
{
    // pid
    int id;
    // ���ȼ�
    int pri;
    // ʹ�ù���ʱ��Ƭ����
    int slice_cnt;
    // Ԥ�ƻ���Ҫ��ʱ��
    int time_need;
    // PC
    int pc;
    // ���С
    int size;
} PCB;

//�ļ��ṹ��
typedef struct XFILE
{
    int id;
}XFILE;

//�������еĿ�ʼʱ��
clock_t system_start;

#endif //SYS_H
