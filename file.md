# 文件操作类FileOperation 需要配合文件管理类FileManager同时使用:
```c++
FileManager fm(512, 200, 20);
FileOperation op(&fm);
```
# cd

实现进入到某个路径的操作

```c++
// 以home目录为根目录，可以输入参数为相对当前显示的工作目录的路径
// 也可以输入参数为以根目录开始的绝对路径
// 如：home里有目录为123，且当前工作目录为"\"，可以通过"\123" 或 "123"进入123目录下
cout << fm.working_path << endl;    // 打印工作目录 （以‘\’开头）
op.cd_command("\\123");
cout << fm.working_path << endl;
op.cd_command("\\");
cout << fm.working_path << endl;
op.cd_command("123\\");
cout << fm.working_path << endl;
op.cd_command("..\\..\\..\\");      // 非法操作支持过滤不处理 
cout << fm.working_path << endl;
op.cd_command("\\666");             // 不存在的目录
cout << fm.working_path << endl;    

// Output:
// \
// \123\
// \
// \123\
// \123\
// cd: '\666' Not such directory.
// \123\
```

# ls

实现展示当前目录下所有文件的操作

```c++
// 以home目录为根目录，可以输入参数为相对当前显示的工作目录的路径
// 也可以输入参数为以根目录开始的绝对路径
// 如：home里有目录为123，且当前工作目录为"\"，显示123目录的内容可以使用 "\123" 或 "123"
// 输入参数为空字符串时，显示当前目录下所有文件

// 在根目录下运行(即 working_path 为 "\")
op.ls_command("");
op.ls_command("\\dir1\\dir2\\dir3");
op.ls_command("dir1\\dir2");
op.ls_command("\\..\\..\\..\\");        // 当超出根目录范围时，只显示根目录下所有文件
op.ls_command("666");

// Output:
// 123       dir1         f1       test      test1      test2  test_file
// dir6    f5
// dir3
// 123       dir1         f1       test      test1      test2  test_file
// ls: '666' No such file or directory

```