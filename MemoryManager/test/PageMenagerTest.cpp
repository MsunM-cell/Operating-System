/*
 * @Date: 2022-04-08 15:22:17
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:23:27
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

    while (!file.eof())
    {
        file.getline(buffer, 256);
        char pid = buffer[0];
        char move = buffer[2];
        switch (move)
        {
        case 'c':
        {
            unsigned long length = 0;
            int index = 4;
            while (buffer[index] < '9' && buffer[index] > '0' && index < 256)
            {
                length = length * 10 + buffer[index] - '0';
                index++;
            }
            int res = PageMemoryManager::getInstance()->memoryAlloc(pid, length);
            if (res == 1)
            {
                cout << "process " << pid << " create , memory Size " << length << " B" << endl;
            }
            else
            {
                cout << "process " << pid << " create fail!" << endl;
            }
            break;
        }

        case 'w':
        {
            unsigned long address = 0;
            int index = 4;
            while (buffer[index] < '9' && buffer[index] > '0' && index < 256)
            {
                address = address * 10 + buffer[index] - '0';
                index++;
            }
            index++;
            string str;
            while (buffer[index] != '\n')
            {
                str += buffer[index];
                index++;
            }
            str += '\0';
            PageMemoryManager::getInstance()->write(address, str.c_str(), str.length(), pid);
            cout << "process " << pid << " write "
                 << "\"" << str << "\" into" << address << endl;
            break;
        }

        case 'r':
        {
            unsigned long address = 0;
            int index = 4;
            while (buffer[index] < '9' && buffer[index] > '0' && index < 256)
            {
                address = address * 10 + buffer[index] - '0';
                index++;
            }
            break;
            char c = PageMemoryManager::getInstance()->accessMemory(pid, address);
            string res;
            while (c != '\0')
            {
                res += 'c';
                address++;
                c = PageMemoryManager::getInstance()->accessMemory(pid, address);
            }
            cout << "process " << pid << " read from " << address << ", result: \"" << res << "\"" << endl;
        }

        case 's':
        {
            PageMemoryManager::getInstance()->stuff(pid);
            break;
        }
        default:
            cout << "没有这条指令:" << move << endl;
            break;
        }
    }

    return 0;
}
