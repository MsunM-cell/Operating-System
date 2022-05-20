#include <iostream>
#include <sstream>
#include <windows.h>

#include "processManager\proc.h"
#include "processManager\proc.cpp"
#include "FileManager\file_manager.h"
#include "FileManager\file_manager.cpp"
#include "FileManager\file_operation.h"
#include "FileManager\file_operation.cpp"
#include "FileManager\json.hpp"
#include "lib\sys.h"
#include "FileManager\json.hpp"
using namespace std;

// 输入缓冲区
string inBuf="";
// 互斥锁
HANDLE hMutex = NULL;
// 信号量
HANDLE hSemaphore = INVALID_HANDLE_VALUE;

/**
 * @brief 一个获取用户输入的函数
 * 
 */
DWORD WINAPI getch(LPVOID lpParamter)
{
    while (inBuf != "exit")
    {
        WaitForSingleObject(hMutex,INFINITE);
        getline(cin,inBuf);
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemaphore, 1, NULL);
    }
    return 0L;
}
/**
 * @brief 进程管理器调度的线程函数，确保可以边接收指令边调度
 * 
 * @param lpParamter 
 * @return DWORD 
 */
DWORD WINAPI setupProcManager(LPVOID lpParamter)
{
    while(true)
    {
        ProcManager::getInstance().scheduling();
        if (ProcManager::getInstance().getActiveNum() == 0)
        {
            Sleep(20);
            ProcManager::getInstance().maintain(20);
        }
    }

    return 0L;
}

/**
 * @brief 对用户指令进行解释
 * 
 * @param cmd 输入指令
 * @param argv 输出的参数数量
 * @return int 参数数量
 */
int parse(string cmd, vector<string> &argv)
{
    istringstream istr(cmd);
    argv.clear();
    string token;

    while (istr >> token)
    {
        argv.push_back(token);
    }
    return argv.size(); 
}

/**
 * @brief 新建pcb
 * 
 * @param file 传入的文件
 */
PCB* createPCB(json file,string path)
{
    PCB* ptr = new PCB;
    ptr->id= ProcManager::getInstance().getAvailableId();
    ptr->path = path;
    ptr->name = file["name"];
    ptr->status = NEW;
    ptr->pc = 0;
    ptr->pri = file["priority"];
    ptr->size = file["size"];
    ptr->slice_cnt = 0;
    ptr->time_need = 5000;
    ptr->cpu_time = 0;
    
    return ptr;
}


int main(void)
{
    string usrname;
    cout << "Please enter your name:";
    cin >> usrname;
    cout << "Welcome, " << usrname << endl;
    hMutex = CreateMutex(NULL, FALSE, NULL);
    hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    // 启动用户输入线程
    HANDLE hThread = CreateThread(NULL, 0, getch, NULL, 0, NULL);
    CloseHandle(hThread);
    // 进程调度线程
    HANDLE procManagerThread = CreateThread(NULL, 0, setupProcManager, NULL, 0, NULL);
    CloseHandle(procManagerThread);
    string cmd="";
    int args=0;
    vector<string> argv;

    // 文件管理初始化
    FileManager fm(512, 200, 12);
    FileOperation fileOperation(&fm);
    string pwd;
    while (cmd != "exit")
    {
        pwd = usrname + "@My-OS:" + fm.working_path + "$ ";
        cout << pwd;
        WaitForSingleObject(hSemaphore, INFINITE);
        WaitForSingleObject(hMutex,INFINITE);
        cmd = inBuf;
        // cout << cmd << endl;
        args = parse(cmd, argv);
        // cout << argv[0];
        // 根据分析出的指令执行相关的操作
        if(args == 0){
            puts("error");
            ReleaseMutex(hMutex);
            // system("pause");
            continue;
            // break;
        }
        if (argv[0] == "ps") {
            if (args == 1)
            {
                ProcManager::getInstance().ps();
            }
            else if (args == 2)
            {
                ProcManager::getInstance().ps(atoi(argv[1].c_str()));
            }
            else
            {
                cout << "unknown cmd!\n";
            }
        }
        else if (argv[0] == "kill")
        {
            if (args == 2)
            {
                ProcManager::getInstance().kill(atoi(argv[1].c_str()));
            }
            else
            {
                cout << "unknown cmd!\n";
            }
        }
        else if(argv[0] == "cm"){
            if (bmm->getMode() == "block")
            {
                bmm->compress_mem();
            }
            else
                cout << "No outer debris, can not compress\n";
        }
        else if (argv[0] == "run")
        {
            if (args == 2)
            {
                json file;
                // cout << fm.working_path << endl;
                string path = fileOperation.pathConverter(argv[1]);
                string get_file_path = fm.working_path + argv[1];
                if (path == "error" || !exists(path)) {
                    puts("error");
                }
                else {
                    cout << path << endl;
                    PCB* pcb;
                    file = fm.get_file(get_file_path, "read", "FCFS");
                    cout << file << endl;
                    pcb = createPCB(file,path);
                    ProcManager::getInstance().run(pcb);
                }
            }
            else
            {
                cout << "unknown cmd!\n";
            }

        }
        else if(argv[0] == "ls"){
            if(args == 2){
                fileOperation.ls_command(argv[1]);
            }
            else{
                fileOperation.ls_command("");
            }
            
        }
        else if (argv[0] == "td")
        {
            fm.tidy_disk();
        }
        else if (argv[0] == "dss")
        {
            fm.display_storage_status();
        }
        else if(argv[0]=="dms")
        {
            bmm->dms_command();
        }
        else if(argv[0] == "cd"){
            fileOperation.cd_command(argv[1]);
        }
        else if (argv[0] == "exit")
        {
            // nop
        }
        else if (argv[0] == "tc")
        {
            json file;
            string path = fm.home_path + fm.working_path + "cpu";
            string get_file_path = fm.working_path + "cpu";
            cout << path << endl;
            PCB* pcb;
            file = fm.get_file(get_file_path, "read", "FCFS");
            for (int i = 0; i < 5; i++)
            {
                pcb = createPCB(file,path);
                ProcManager::getInstance().run(pcb);
            } 
        }  
        else if(argv[0] == "rm"){
            if(args == 2 && argv[1] != "-r"){
                string dir = "";
                string file_name = "";
                if (argv[1].front() == path::preferred_separator) {
                    dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                    file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                } else {
                    if (argv[1].find(path::preferred_separator) == string::npos) {
                        dir = ".";
                        file_name = argv[1];
                    }
                    else {
                        dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                        file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                    }
                }
                fileOperation.delete_file(dir, file_name);
            }
            else if (args == 3) {
                bool recursive = false;
                int index = -1;

                for (int i = 0; i < 3; i++) 
                    if (argv[i] == "-r") {
                        recursive = true;
                        index = i;
                        break;
                    }
                
                if (!recursive) {
                    puts("error");
                } else {
                    
                    if (index == 1) {
                        string dir = "";
                        string file_name = "";
                        if (argv[2].front() == path::preferred_separator) {
                            dir = argv[2].substr(0, argv[2].find_last_of(path::preferred_separator));
                            file_name = argv[2].substr(argv[2].find_last_of(path::preferred_separator) + 1);
                        } else {
                            if (argv[2].find(path::preferred_separator) == string::npos) {
                                dir = ".";
                                file_name = argv[2];
                            }
                            else {
                                dir = argv[2].substr(0, argv[2].find_last_of(path::preferred_separator));
                                file_name = argv[2].substr(argv[2].find_last_of(path::preferred_separator) + 1);
                            }
                        }
                        fileOperation.delete_dir(dir, file_name);
                    }
                    else {
                        string dir = "";
                        string file_name = "";
                        if (argv[1].front() == path::preferred_separator) {
                            dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                            file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                        } else {
                            if (argv[1].find(path::preferred_separator) == string::npos) {
                                dir = ".";
                                file_name = argv[1];
                            }
                            else {
                                dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                                file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                            }
                        }
                        fileOperation.delete_dir(dir, file_name);
                    }
                }

            }
            else{
                puts("error");
            }
        }
        else if(argv[0] == "mkdir"){
            if(args == 2){
                string dir = "";
                string dir_name = "";
                if (argv[1].front() == path::preferred_separator) {
                    dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                    dir_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                } else {
                    if (argv[1].find(path::preferred_separator) == string::npos) {
                        dir = ".";
                        dir_name = argv[1];
                    }
                    else {
                        dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                        dir_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                    }
                }
                fileOperation.create_dir(dir, dir_name);
            }
            else{
                puts("error");
            }
        }
        else if(argv[0] == "touch"){
            if(args == 2){
                string dir = "";
                string file_name = "";
                if (argv[1].front() == path::preferred_separator) {
                    dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                    file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                } else {
                    if (argv[1].find(path::preferred_separator) == string::npos) {
                        dir = ".";
                        file_name = argv[1];
                    }
                    else {
                        dir = argv[1].substr(0, argv[1].find_last_of(path::preferred_separator));
                        file_name = argv[1].substr(argv[1].find_last_of(path::preferred_separator) + 1);
                    }
                }
                fileOperation.create_file(dir, file_name);
            }
            else{
                puts("error");
            }
        }
        else if(argv[0] == "cp"){
            if(args == 3){
                fileOperation.copy_file(argv[1], argv[2]);
            }
            else{
                puts("error");
            }
        }
        else if(argv[0] == "mv"){
            if(args == 3){
                fileOperation.move_file(argv[1], argv[2]);
            }
            else{
                puts("error");
            }
        }
        else if (argv[0] == "chmod") {
            if (args == 3) {
                fileOperation.modify_file_type(argv[1], 0 + argv[2][0]);
            }
            else {
                puts("error");
            }
        }
        else
        {
            cout << "unknown cmd!\n";
        }
        ReleaseMutex(hMutex);
    }
    return 0;
}
