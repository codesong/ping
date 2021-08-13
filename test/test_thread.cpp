/*************************************************************************
* File Name: test_thread.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 08时40分23秒
*************************************************************************/

#include <iostream>
#include "../src/Mutex.h"
#include "../src/Thread.h"
#include "../src/TimeStamp.h"

using namespace ping;

void test_thread()
{
    for(int i = 0; i < 5; ++i)
    {
        std::cout << "hello codesong!" << std::endl;
        TimeStamp ts;
        std::cout << ts.toString(YYYYMMDD) << std::endl;
    }
}

int main(int argc, char *argv[])
{
    Thread t(test_thread, "test thread");
    t.start();

    return 0;
}
