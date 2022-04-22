#include <iostream>
#include <sstream>
#include <windows.h>


#include "processManager\proc.cpp"
#include "lib\sys.h"
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

DWORD WINAPI setupProcManager(LPVOID lpParamter)
{
    ProcManager::getInstance().scheduling();
    return 0L;
}

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


int main(void)
{
    hMutex = CreateMutex(NULL, FALSE, NULL);
    hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    HANDLE hThread = CreateThread(NULL, 0, getch, NULL, 0, NULL);
    CloseHandle(hThread);
    HANDLE procManagerThread = CreateThread(NULL, 0, setupProcManager, NULL, 0, NULL);
    CloseHandle(procManagerThread);
    string cmd="";
    int args=0;
    vector<string> argv;

    while (cmd != "exit")
    {
        WaitForSingleObject(hSemaphore, INFINITE);
        WaitForSingleObject(hMutex,INFINITE);
        cmd = inBuf;
        cout << cmd << endl;
        args = parse(cmd, argv);
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