/**
 * ϵͳ����ͷ�ļ�
 */

#ifndef SYS_H
#define SYS_H

#define SYS_NAME 114


// PCB�Ľṹ��
typedef struct
{
    // pid
    int id;
    // ���ȼ�
    int pri;
    // ʹ�ù���ʱ��Ƭ����
    int slice_cnt;
    // Ԥ�ƻ���Ҫ��ʱ��
    int time_need;
} PCB;

//�ļ��ṹ��
typedef struct
{
    int id;
}XFILE;


//�������еĿ�ʼʱ��
clock_t system_start;

#endif //SYS_H
