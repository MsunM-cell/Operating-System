/*
 * @Date: 2022-04-01 16:36:01
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:19:20
 * @FilePath: \Operating-System\MemoryManager\include\config.h
 */
#define LOGICAL_MEMORY_SIZE (16<<20)     //16MB
#define PAGE_SIZE (4 << 10)              //4KB
#define PHYSICAL_MEMORY_SIZE (1024 * PAGE_SIZE) // 4MB
#define SWAP_MEMORY_SIZE (1024 * PAGE_SIZE)    //4MB
#define PAGE_NUM 2048
#define FRAME_NUM 1024
#define IS_DEBUG 1



