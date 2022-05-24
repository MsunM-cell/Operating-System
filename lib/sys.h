/**
 * 系统共用头文件
 */

#include<ctime>

#ifndef SYS_H
#define SYS_H

#define SYS_NAME 114
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define NEW 4
#define DEV_NUM 1



// PCB的结构体
typedef struct PCB
{
    // pid
    int id;
    // 进程名称
    std::string path;
    // 进程状态
    int status;
    // 优先级
    int pri;
    // 使用过的时间片数
    int slice_cnt;
    // 预计还需要的时间
    int time_need;
    // 预计阻塞的时间
    int block_time;
    // 还需要使用cpu的时间
    int cpu_time;
    // PC
    int pc;
    // 块大小
    int size;
    //页表首地址
    int pagetable_addr;
    //页表长度
    int pagetable_len;
    std::string name;
} PCB;

//文件结构
typedef struct XFILE
{
    // 文件名称
    std::string name;
    // 文件类型
    std::string type;
    // 文件大小
    unsigned long size;
    // 文件内容
    std::string content;
}XFILE;

//程序运行的开始时间
clock_t system_start;

#endif //SYS_H
