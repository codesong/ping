/*************************************************************************
* File Name: test_log.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月16日 星期一 13时37分38秒
*************************************************************************/

#include "../src/Log.h"

using namespace ping;

void test_log()
{
    for(int i = 0; i < 100000; ++i)
    {
        LOG_INFO << "hello codesong!";
        LOG_INFO << "Across the Great Wall we can reach every corner in the world.";
    }
}

int main(int argc, char *argv[])
{
    LOGGER.init("./log", argv[0], TRACE);
    test_log();
    test_log();
    test_log();
    test_log();
    test_log();
    LOG_FATAL << "fatal error.";
    return 0;
}

