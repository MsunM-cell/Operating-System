/*
 * @Author: LuChang
 * @Date: 2022-03-26 13:29:50
 * @LastEditors: LuChang
 * @LastEditTime: 2022-03-26 15:41:18
 * @FilePath: \osDesign\OS\Operating-System\processManager\FCFS.cpp
 * @Description: 
 *      processmanager,using FCFS algorithm
 * Copyright (c) 2022 by LuChang, All Rights Reserved. 
 */

#include <iostream>
#include "proc.h"
class procManagerFCFS{
    public:
        void addToQueue(PCB p);
        void runProcManager();
        void run(PCB p);
    private:
        queue <PCB> fcfsQueue;
    
};

void procManagerFCFS::addToQueue(PCB p){
    fcfsQueue.push(p);
}

void procManagerFCFS::runProcManager(){
    while(true){
        while(!fcfsQueue.empty()){
            PCB p = fcfsQueue.front();
            //该函数是执行函数，暂时未定
            run(p);
            fcfsQueue.pop();
        }
    }
}

void procManagerFCFS::run(PCB p){
    std::cout << "process " << p.id << " is running" << endl;
}