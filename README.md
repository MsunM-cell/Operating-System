# Operating System

## 文件结构
``` text
|-- os-sys
    |-- README.md
    |-- run.sh
    |-- FileManager
    |   |-- file_manager.cpp
    |   |-- file_manager.h
    |   |-- file_operation.hpp
    |   |-- json.hpp
    |-- lib 系统共用头文件目录
    |   |-- sys.h
    |-- MemoryManager
    |   |-- README.txt
    |   |-- include
    |   |   |-- config.h
    |   |   |-- FrameTableItem.h
    |   |   |-- PageMemoryManager.h
    |   |   |-- PageTable.h
    |   |   |-- PageTableItem.h
    |   |   |-- ProcessTableItem.h
    |   |   |-- tableItem.h
    |   |-- Interface
    |   |   |-- FileManager.cpp
    |   |   |-- MemoryManager-1.cpp
    |   |   |-- MemoryManager.cpp
    |   |-- Manager
    |   |   |-- BasicPageManager.cpp
    |   |   |-- DynamicBlockManager.cpp
    |   |   |-- FrameTableItem.cpp
    |   |   |-- Log.cpp
    |   |   |-- memory.h
    |   |   |-- MyFileManager.cpp
    |   |   |-- PageMemoryManager.cpp
    |   |-- test
    |       |-- MemoryUsage.txt
    |       |-- MyFileManagerTest.cpp
    |       |-- MyFileManagerTest.exe
    |       |-- PageManagerTest-MutiThread.cpp
    |       |-- PageManagerTest-MutiThread.exe
    |       |-- PageMenagerTest.cpp
    |       |-- PageMenagerTest.exe
    |       |-- testProgram.txt
    |-- processManager 进程管理器
        |-- proc.cpp
        |-- proc.h

```

## 类图

```mermaid
classDiagram
class RRQueue {
    -vector~PCB*~ rr_que
    +getSize() int 
    +addPCB(PCB* target) bool
    +removePCB(int pid) bool
    +downLevel(PCB* target,ProcManagerFCFS*fcfs) void 
    +scheduling(ProcManagerFCFS* fcfs) int
    +getInfo() void
    +getInfo(int pid) void
}
class ProcManagerFCFS {
    -vector ~PCB*~ fcfsQueue
    +addToQueue(PCB *p) void
    +runProcManager() void
    +removeProc(int pid) bool
    +getFcfsInfo() void
    +getFcfsInfo(int pid) void
    +getQueueSize() int
    -run(PCB *p) void
    -getCommand() string
}
class ProcManager {
    -int cpid
    -vector~PCB*~ active_pcb
    -vector~PCB*~ waiting_pcb
    -RRQueue* rr_queue
    -ProcManagerFCFS* fcfsProcManager

    +getActiveNum() int
    +kill(int pid) void
    +run(string file_name) void
    +ps() void
    +ps(int pid) void
    +scheduling() void
    +freePCB(PCB* target) bool
    +maintain() void
    +getInstance()$
}
ProcManager *-- RRQueue
ProcManager *-- ProcManagerFCFS
RRQueue ..>ProcManagerFCFS 
```

```mermaid
classDiagram
class FileManager {
    +string home_path
    +string working_path
    -Disk disk
    -vector~Block~ blocks
    -vector~int~ bitmap
    -json file_system_tree
    -json file_blocks

    +init_blocks() void
    +init_file_system_tree(string current_path) json
    +fill_file_into_blocks(json file_info, string file_path, int method) bool
    +delete_file_from_blocks(string file_path) bool
    +find_idle_blocks(int num, int method) int
    +first_fit(string target_str) int
    +best_fit(string target_str) int
    +worst_fit(string target_str) int
    +add_json_node_to_tree(string path, json node) bool
    +delete_json_node_from_tree(string path) bool
    +get_file(string file_path, string mode, string seek_algo) json
}

class FileOperation {
    -FileManager* file_manager

    +create_file(string current_dir, string file_name) bool
    +delete_file(string current_dir, string file_name) bool
    +create_dir(string currenet_dir, string dir_name) bool
    +delete_dir(string current_dir, string dir_name) bool
    +tree(string dir, int layer = 0) void
}

class Block {
    -int total_space
    -int free_space
    -int track
    -int sector
    -string fp

    +set_free_space(int free_space) void
    +set_fp(string fp) void
    +get_location() pair<int, int>
}

class Disk {
    -int sector_size
    -int track_num
    -int track_size
    -int head_pointer
    -double seek_speed
    -double rotate_speed
    -int slow_ratio

    +set_head_pointer(int head_pointer) void
    +seek_by_queue(vector<pair<int, int>> seek_queue) void
    +FCFS(vector<pair<int, int>> seek_queue) void
    +SSTF(vector<pair<int, int>> seek_queue) void
}

FileManager *-- Disk
FileManager *-- Block
FileManager *-- FileOperation
FileOperation ..> Disk
FileOperation ..> Block
Disk ..> Block

```

## 存在的问题

### 内存
**内存部分完成整合，目前存在的问题如下：**

    1. 头文件的引用较严格，外部引用只能 include"MemoryManager.cpp" (Manager文件夹下那个),否则编译不通过。
    2. 测试单独的Manager目前不能直接在类cpp下文件写main函数，否则会因为没有include"MemoryManager.cpp"导致编译失败，解决办法是另开一个cpp文件include"MemoryManager.cpp"后进行测试。

**外部使用的方法如下：**

```cpp
#include "../Manager/MemoryManager.cpp"
int main()
{
    MemoryManager *manager = MemoryManager::getInstance();
    PCB *new_pcb = new PCB;
    manager->createProcess((*new_pcb));
    manager->accessMemory(1,1234567);
    manager->freeProcess(*new_pcb);
    delete manager;
}
```

**修改getInstance()返回的不同manager的方法为：**

    修改cfg下Page、Virtual Memory选项。
    
    其中Page选项与Virtual Memory同时为yes时为有虚拟内存的页式管理
    仅Page选项为yes、Virtual Memory为no时为基本分页
    两者均为no时为连续分配
    默认：连续分配

     
**注意**

    调用完毕子类析构方法后会调用父类的析构方法，父类中析构方法已经有内存释放操作，子类中析构方法不要重复释放




### 文件

### 进程


## TODO

    监控各组件运行情况接口

## TIPS

1. 我添加了一个叫`sys.h`的头文件在`lib`目录下，如果想调用这个公共的头文件，可以在自己模块的头文件下加入`#include "../lib/sys.h"`

2. 运行`file_manager.cpp`需配合`-std=c++17`食用，可打印如下文件树：

<img width="123" alt="截屏2022-04-02 上午1 58 54" src="https://user-images.githubusercontent.com/58599953/161317426-bd06a47b-05c5-4d4b-bb50-4591d917f34e.png">
