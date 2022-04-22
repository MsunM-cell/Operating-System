//test vscode git
一个简单的内存控制指令。
第一列为pid,第二列为指令,后继为指令的参数，具体有为：
pid  c size           : 创建(Create)进程，size为进程的大小，单位为Byte
pid  s                : 污染(Stuff)进程的所有页,crete指令仅仅给了进程访问权限，读写操作能分配真正的内存，方便检验页面置换。
pid  w addr massage#  ：写入(Write)操作，size为进程的逻辑地址，msg以#结尾（暂时这么判定）
pid  r addr           ：读出(Read)操作，address为进程的逻辑地址
pid  f                : 释放该进程所有内存

可以运行test/PageManagerTest进行测试

指令示意如下
1 c 2097152
2 c 2097152
3 c 2097152
1 s 
2 s 
1 w 2080000 1: Hello world!#
1 w 1036000 1: Welcome!#
2 w 1450698 2: Operating System#
3 w 1425368 3: Memory Manager#
3 s 
1 r 1036000
2 r 1450698
3 r 1425368
3 f
2 f
1 f
