/*************************************************************************
* File Name: Util.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 21时42分12秒
*************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "Timestamp.h"

namespace ping
{

using std::string;

extern thread_local pid_t t_threadId;
extern thread_local string t_threadName;

namespace Util
{
    pid_t currThreadId();
    const string currThreadName();
    bool isMainThread();
    Timestamp currTime();

} // namespace Util

} // namespace ping

#endif
