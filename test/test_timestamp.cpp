/*************************************************************************
* File Name: test_timestamp.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月11日 星期三 06时20分47秒
*************************************************************************/

#include <string>
#include <iostream>
#include "../src/Util.h"
#include "../src/Timestamp.h"
#include "../src/Exception.h"

using namespace ping;

void test_timestamp()
{
    Timestamp tt = Util::currTime();
    std::cout << tt.toString(YYYY) << std::endl;
    std::cout << tt.toString(YYYYMM) << std::endl;
    std::cout << tt.toString(YYYYMM2) << std::endl;
    std::cout << tt.toString(YYYYMMDD) << std::endl;
    std::cout << tt.toString(YYYYMMDD2) << std::endl;
    std::cout << tt.toString(YYYYMMDDHH) << std::endl;
    std::cout << tt.toString(YYYYMMDDHH2) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMM) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMM2) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMMSS) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMMSS2) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMMSSMS) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMMSSMS2) << std::endl;
    std::cout << tt.toString(YYYYMMDDHHMMSSMS3) << std::endl;
};

int main(int argc, char *argv[])
{
    test_timestamp();
    return 0;
}

