/*
 * @Date: 2022-04-08 15:22:17
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 15:23:27
 * @FilePath: \Operating-System\MemoryManager\test\PageMenagerTest.cpp
 */
#include "../Manager/PageMemoryManager.cpp"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
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
            int res = PageMemoryManager::getInstance()->memoryAlloc(pid, length);
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
            PageMemoryManager::getInstance()->write(address, str.c_str(), str.length(), pid);
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
            PageMemoryManager::getInstance()->stuff(pid);
            break;
        }
        default:
            stringstream ss;
            ss << "instruction \"" << move << "\" NOT found !";
            Log::logI(TAG, ss.str());
            break;
        }
    }

    return 0;
}
