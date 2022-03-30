/**
 * 系统共用头文件
 */

#ifndef SYS_H
#define SYS_H

#define SYS_NAME 114


// PCB的结构体
typedef struct PCB
{
    // pid
    int id;
    // 优先级
    int pri;
    // 使用过的时间片数量
    int slice_cnt;
    // 预计还需要的时间
    int time_need;
    // PC
    int pc;
    // 块大小
    int size;
} PCB;

//文件结构体
typedef struct XFILE
{
    int id;
}XFILE;

//程序运行的开始时间
clock_t system_start;

#endif //SYS_H
