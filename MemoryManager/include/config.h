/*
 * @Date: 2022-04-01 16:36:01
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-13 15:18:37
 * @FilePath: \Operating-System\MemoryManager\include\config.h
 */

#define PAGE_SIZE (4 << 10)                     // 4KB
#define PHYSICAL_MEMORY_SIZE (1024 * PAGE_SIZE) // 4MB
#define SWAP_MEMORY_SIZE (1024 * PAGE_SIZE)     // 4MB
#define PAGE_NUM 2048
#define FRAME_NUM 1024
#define LOGICAL_MEMORY_SIZE (PHYSICAL_MEMORY_SIZE + SWAP_MEMORY_SIZE) // 16MB