/*
 * @Date: 2022-04-08 16:51:14
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-04-09 14:59:53
 * @FilePath: \Operating-System\MemoryManager\Manager\Log.cpp
 */

#ifndef LOG_UTIL
#define LOG_UTIL

#include <iostream>
#include <string>
#include <sstream>
// #include <windows.h>

int log_mode = 0;
int last_log_mode = -1;

class Log
{
private:
    /* data */
public:
    static const int DEBUG = 0;
    static const int VERBOSE = 1;
    static const int INFO = 2;
    static const int ERRORR = 3; //不知道和哪个库重名了....多加一个R
    static const int SUPPRESS = 100; //不输出任何信息

    static void logV(std::string TAG, std::string msg)
    {
        if (log_mode <= VERBOSE)
        {
            logcat(TAG, msg);
        }
    }

    static void logI(std::string TAG, std::string msg)
    {
        if (log_mode <= INFO)
        {
            logcat(TAG, msg);
        }
    }

    static void logE(std::string TAG, std::string msg)
    {
        if (log_mode <= ERRORR)
        {
            logcat(TAG, msg);
        }
    }

    /**
     * @description: 仅仅用于后续方便地切换debug输出
     * @param {string} TAG
     * @param {string} msg
     * @return {*}
     */
    static void logcat(std::string TAG, std::string msg)
    {
        // std::string time = getTimeString();

        std::cout  << TAG << " : " << msg << std::endl;
    }

    // static std::string getTimeString()
    // {
    //     SYSTEMTIME t;
    //     GetLocalTime(&t);
    //     std::stringstream res;
    //     // res << t.wYear << "-" << t.wMonth << "-" << t.wDay << " ";
    //     res << t.wHour << ":" << t.wMinute << ":" << t.wSecond << "." << t.wMilliseconds << " ";
    //     return res.str();
    // }

    static void stopLog(){
        last_log_mode = log_mode;
        log_mode = ERRORR;
    }

    static void continueLog(){
        log_mode = last_log_mode;
    }
};

#endif