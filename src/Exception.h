/*************************************************************************
* File Name: Exception.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月12日 星期四 04时50分35秒
*************************************************************************/

#ifndef __CHECK_H__
#define __CHECK_H__

#include <string>
#include <stdexcept>
#include "Noncopyable.h"

namespace ping
{

#define CHECK(condition) \
    if(!(condition)) { throw std::logic_error("check failed: " #condition); }

#define CHECK_RETZERO(FUNC) CHECK(0 == FUNC)

using std::string;

class Exception: Noncopyable
{

private:
public:
    string stackTrace();
};

}

#endif

