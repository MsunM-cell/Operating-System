/*
 * @Date: 2022-04-13 15:22:51
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-07 22:39:12
 * @FilePath: \Operating-System\MemoryManager\test\PageManagerTest-MutiThread.cpp
 * @copyright: Copyright (C) 2022 shimaoZeng. All rights reserved.
 */

#include <fstream>
#include "../Manager/PageMemoryManager.cpp"
#include <thread>
#include <iomanip>

void test();
void monitor();

int STOP = 0;

int main()
{
    thread testThread(test);
    thread monitorThread(monitor);
    testThread.join();
    monitorThread.join();
    return 0;
}

void test()
{
    ifstream file;
    file.open("testProgram.txt", ios::in);
    char *buffer = new char[256];
    string TAG = "TestShell";
    while (!file.eof())
    {

        file.getline(buffer, 256);
        int pid = buffer[0] - '0';
        char move = buffer[2];
        switch (move)
        {
        case 'c':
        {

            long long length = 0;
            int index = 4;
            while (buffer[index] <= '9' && buffer[index] >= '0' && index < 256)
            {
                length = length * 10 + buffer[index] - '0';
                index++;
            }
            PCB* p = new PCB;
            p->id = pid;
            p->size = length;
            int res = PageMemoryManager::getInstance()->createProcess(*p);
            delete p;
            if (res == 1)
            {
                stringstream ss;
                ss << "process " << pid << " create , memory size " << length << "B";
                Log::logI(TAG, ss.str());
            }
            else
            {
                stringstream ss;
                ss << "process " << pid << " create fail!";
                Log::logI(TAG, ss.str());
            }
            break;
        }

        case 'w':
        {
            long long address = 0;
            int index = 4;
            while (buffer[index] <= '9' && buffer[index] >= '0' && index < 256)
            {
                address = address * 10 + buffer[index] - '0';
                index++;
            }
            index++;
            string str;
            while (buffer[index] != '#')
            {
                str += buffer[index];
                index++;
            }
            str += '#';
            PageMemoryManager::getInstance()->writeMemory(address, str.c_str(), str.length(), pid);
            break;
        }

        case 'r':
        {
            long long address = 0;
            int index = 4;
            while (buffer[index] <= '9' && buffer[index] >= '0' && index < 256)
            {
                address = address * 10 + buffer[index] - '0';
                index++;
            }
            char c = PageMemoryManager::getInstance()->accessMemory(pid, address);
            string res;
            int time = 0;
            long long temp_address = address;
            while (c != '#' && time < 100)
            {
                res += c;
                temp_address++;
                time++;
                c = PageMemoryManager::getInstance()->accessMemory(pid, temp_address);
            }
            stringstream ss;
            ss << "process " << pid << " read from logical address " << address << ", result: \"" << res << "\"";
            Log::logI(TAG, ss.str());
            break;
        }

        case 's':
        {
            stringstream ss;
            ss << "stuff all pages of process " << pid << endl;
            Log::logI(TAG, ss.str());
            PageMemoryManager::getInstance()->stuff(pid);
            break;
        }
        case 'f':
        {
            stringstream ss;
            ss << "delete process  " << pid << endl;
            Log::logI(TAG, ss.str());
            PCB* p = new PCB;
            p->id = pid;
            PageMemoryManager::getInstance()->freeProcess(*p);
            delete p;
            break;
        }
        default:
            stringstream ss;
            ss << "instruction \"" << move << "\" NOT found !";
            Log::logI(TAG, ss.str());
            break;
        }
    }
    STOP = 1;
}

void monitor()
{

    ofstream log;
    log.open("MemoryUsage.txt", ios::out);
    log << setw(12) << left << "time"
        << " "
        << setw(6) << "submit"
        << " "
        << setw(9) << "usedFrame"
        << " "
        << setw(7) << "swapped"
        << " " << endl;
    PageMemoryManager *manager = PageMemoryManager::getInstance();
    while (!STOP)
    {
        log << setw(12) << Log::getTimeString() << " "
            << setw(6) << manager->getOccupiedPageNum() << " "
            << setw(9) << manager->getUsedFrameNum() << " "
            << setw(7) << manager->getSwapPageNum() << endl;
    }
}