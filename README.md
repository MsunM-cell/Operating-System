# Operating System

## 类图

```mermaid
classDiagram
class RRQueue {
    -vector~PCB*~ rr_que
    +getSize() int 
    +addPCB(PCB* target) bool
    +removePCB(int pid) bool
    +downLevel(PCB* target,ProcManagerFCFS*fcfs) +void 
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
    +getInstance()$
}
ProcManager *-- RRQueue
ProcManager *-- ProcManagerFCFS
RRQueue ..>ProcManagerFCFS 
```

## 存在的问题

### 内存

### 文件

### 进程

- [ ] fcfs对pcb的释放问题

## TODO

- [ ] 4-14中期验收

## TIPS

1. 我添加了一个叫`sys.h`的头文件在`lib`目录下，如果想调用这个公共的头文件，可以在自己模块的头文件下加入`#include "../lib/sys.h"`

2. 运行`file_manager.cpp`需配合`-std=c++17`食用，可打印如下文件树：

<img width="123" alt="截屏2022-04-02 上午1 58 54" src="https://user-images.githubusercontent.com/58599953/161317426-bd06a47b-05c5-4d4b-bb50-4591d917f34e.png">
