/*
 * @Date: 2022-04-08 16:51:14
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-08 17:25:27
 * @FilePath: \Operating-System\MemoryManager\Manager\Log.cpp
 */
#include <iostream>
#include <string>
#include "../include/config.h"
class Log
{
private:
    /* data */
public:
    /**
     * @description: 仅仅用于后续方便地切换debug输出
     * @param {string} TAG
     * @param {string} msg
     * @return {*}
     */
    static void dbug(std::string TAG, std::string msg)
    {
        if (IS_DEBUG == 1)
        {
            std::cout << TAG << " : " << msg << std::endl;
        }
    }
};
