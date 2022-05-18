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

# rm (rm仅仅只使用于单个文件 若需要删除文件夹请使用rm -r)
删除文件或文件夹

```c++
// 如果删除home下的test文件 假设当前工作目录在"\" 需要以下操作
bool ans = op.delete_file(".", "test");
bool ans = op.delete_file(".\\", "test");
bool ans = op.delete_file("\\", "test");
// 以上三个代码都可以实现
// 即第一个参数是os中的路径，第二个参数是文件名 第一个参数最后的斜杠可加可不加

```

```c++
// 如果删除home下的dir1文件夹，假设当前处于工作目录"\" 需要以下操作
bool ans = op.delete_dir(".", "dir1");   // 当前处于home目录下
bool ans = op.delete_dir(".\\", "dir1");     // 当前处于home目录下
bool ans = op.delete_dir("\\", "dir1");
// 以上三个代码都可以实现
// 即第一个参数是待删除目录所属的目录的os中的路径，第二个参数是目录名 第一个参数最后的斜杠可加可不加
```

# mkdir
创建目录

```c++
// 假设想给home目录下创建一个新目录"new_dir"
bool ans = op.create_dir("\\", "new_dir");
bool ans = op.create_dir(".", "new_dir");   // 当前处于home目录下
// 以上两个代码都能实现
// 即第一个为待新建目录所属的目录的os中的路径，第二个参数是目录名
```

# touch 
创建文件

```c++
// 假设在home的dir1目录下创建一个 "hhh"文件
bool ans = op.create_file("\\dir1", "hhh");
bool ans = op.create_file("dir1", "hhh");   // 当前处于home目录下
// 以上两个代码都能实现
// 即第一个为待新建文件所属的目录的os中的路径，第二个参数是文件名
```

# cp
复制文件

```c++
// 假设将home下的test复制到dir1下
bool ans = op.copy_file("test", "dir1");
bool ans = op.copy_file("\\test", "\\dir1");
bool ans = op.copy_file(".\\test", "dir1\\hhh"); // 这种参数为复制的同时改名
//以上三种代码都可以实现，第一个参数必须为需要复制的文件的os中的路径且最后不能有斜杠，第二个参数可以是需要复制的目录或者是一个具有新文件名的os中的路径
```

# mv
移动文件
参数使用和cp一致

# chmod
修改文件权限

shell中使用方法为chmod [file_path] [mode]
```c++
// 假设需要修改home下的test的权限，变为不可读不可写不可执行
bool ans = op.modify_file_type("test", 0);      // 当前处于home目录下
// 假设需要修改home下的test的权限，变为可读可写可执行
bool ans = op.modify_file_type("\\test", 7);
// 即以二进制从左到右读写执行，为每一位为1则表示拥有权限
// 可读可写不可执行的二进制为110，mode就是6
```