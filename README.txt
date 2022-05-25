Welcome to README of Mini OS.

注意事项：
1. 用户直接运行main.exe可以使用模拟OS系统，或编译运行main.cpp文件（需要高版本gcc以及c++17参数）。

2. MemoryManager/Manager下的cfg文件是内存的配置文件：
“Block_algorithm”可选FF（首次适应）或BF（最佳适应）
“Page”可选yes/no表示是否开启页式管理，否则是动态分区管理
“Page_size”是页大小，用户可以自己设定，默认1KB/页（注意：内存大小固定是32KB）
“Swap_memory_size”是交换区大小，用户同样可以自己设定
“Virtual_memory”可以是yes/no表示是否开启虚拟内存

3. processManager下的debug.txt文件是进程活动的日志文件，在main运行参数不加-d1时可以记录一次运行的进程创建、运行等活动。

4. memorylog.txt文件是内存监视的日志文件，自OS系统运行，每个500ms自动保存一条内存信息记录

5. home目录是模拟的OS系统的根目录



