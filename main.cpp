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

// 输入缓冲�??
string inBuf="";
// 互斥�??
HANDLE hMutex = NULL;
// 信号�??
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
 * @brief 进程管理器调度的线程函数，确保可以边接收指令边调�??
 * 
 * @param lpParamter 
 * @return DWORD 
 */
DWORD WINAPI setupProcManager(LPVOID lpParamter)
{
    // ProcManager::getInstance().scheduling();
    while(true)
    {
        ProcManager::getInstance().scheduling();
        if (ProcManager::getInstance().getActiveNum() == 0)
        {
            Sleep(200);
        }
    }

    return 0L;
}

/**
 * @brief 对用户指令进行解�??
 * 
 * @param cmd 输入指令
 * @param argv 输出的参数数�??
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
 * @param file 传入的文�??
 */
PCB* createPCB(json file)
{
    PCB* ptr = new PCB;
    // ptr->id=;
    ptr->name = file["name"];
    // ptr->pc=0;
    // ptr->pri=;
    // ptr->size=;
    // ptr->slice_cnt=0;
    // ptr->time_need=;
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

    // 文件管理�??
    FileManager fm(512, 200, 12);
    FileOperation fileOperation(&fm);
    while (cmd != "exit")
    {
        WaitForSingleObject(hSemaphore, INFINITE);
        WaitForSingleObject(hMutex,INFINITE);
        cmd = inBuf;
        // cout << cmd << endl;
        args = parse(cmd, argv);
        // cout << argv[0];
        // 根据分析出的指令执行相关的操�??
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
        else if (argv[0] == "run")
        {
            // 真实模式
            if (args == 2)
            {
                json file;
                string path = fm.working_path + argv[1];
                PCB* pcb;
                // ProcManager::getInstance().run(argv[1]);
                file = fm.get_file(path, "read", "FCFS");
                cout << file << endl;
                // 判断有没有x
                pcb = createPCB(file);
                // TODO 怎么引用
                // createProcess();
            }
            // 测试�??
            // else if (args == 3)
            // {
            //     ProcManager::getInstance().run(argv[1], atoi(argv[2].c_str()));
            // }
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
        else if(argv[0] == "cd"){
            fileOperation.cd_command(argv[1]);
        }
        else if (argv[0] == "exit")
        {
            // nop
        }
        else
        {
            cout << "unknown cmd!\n";
        }
        ReleaseMutex(hMutex);
    }
    return 0;
}