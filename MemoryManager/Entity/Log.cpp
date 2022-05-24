/*
 * @Date: 2022-04-08 16:51:14
 * @LastEditors: ShimaoZ
 * @LastEditTime: 2022-05-24 12:24:00
 * @FilePath: \Operating-System\MemoryManager\Entity\Log.cpp
 */

#ifndef LOG_UTIL
#define LOG_UTIL

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>

int log_mode = 3;
int last_log_mode = -1;

class Log
{
private:
    /* data */
public:
    static const int DEBUG = 0;
    static const int VERBOSE = 1;
    static const int INFO = 2;
    static const int ERRORR = 3;     //不知道和哪个库重名了....多加一个R
    static const int SUPPRESS = 100; //不输出任何信息

    /**
     * @brief : 日志输出等级为Verbose
     * @param {string} TAG 标签
     * @param {string} msg 日志
     */
    static void logV(std::string TAG, std::string msg)
    {
        if (log_mode <= VERBOSE)
        {
            logcat(TAG, msg);
        }
    }

    /**
     * @brief : 日志输出等级为Infomation
     * @param {string} TAG 标签
     * @param {string} msg 日志
     */
    static void logI(std::string TAG, std::string msg)
    {
        if (log_mode <= INFO)
        {
            logcat(TAG, msg);
        }
    }

    /**
     * @brief : 日志等级为Error
     * @param {string} TAG 标签
     * @param {string} msg 日志
     */
    static void logE(std::string TAG, std::string msg)
    {
        if (log_mode <= ERRORR)
        {
            logcat(TAG, msg);
        }
    }

    /**
     * @brief 统一日志格式及输出接口，后期方便统一修改
     * @param {string} TAG 标签
     * @param {string} msg 日志
     * @return {*}
     */
    static void logcat(std::string TAG, std::string msg)
    {
        std::string time = getTimeString();

        std::cout << time << " " << TAG << " : " << msg << std::endl;
    }

    static std::string getTimeString()
    {
        SYSTEMTIME t;
        GetLocalTime(&t);
        std::stringstream res;
        // res << t.wYear << "-" << t.wMonth << "-" << t.wDay << " ";
        res << t.wHour << ":" << t.wMinute << ":" << t.wSecond << "." << t.wMilliseconds << " ";
        return res.str();
    }

    static void stopLog()
    {
        last_log_mode = log_mode;
        log_mode = ERRORR;
    }

    static void continueLog()
    {
        log_mode = last_log_mode;
    }

    static void setLogMode(int logmode){
        log_mode = logmode;
    }
};

#endif