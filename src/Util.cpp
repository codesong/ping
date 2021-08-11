/*************************************************************************
* File Name: Util.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 22时23分26秒
*************************************************************************/

#include "Util.h"

namespace ping
{

thread_local pid_t t_threadId = 0;
thread_local string t_threadName;

pid_t Util::CurrThreadId()
{
    if(t_threadId <= 0)
    {
        t_threadId = static_cast<pid_t>(::syscall(SYS_gettid));
    }

    return t_threadId;
}

const string Util::CurrThreadName()
{
    if(t_threadName.empty())
    {
        pid_t currThreadId = CurrThreadId();
        t_threadName = "Thread" + std::to_string(currThreadId);
    }
    return t_threadName;
}


} // namespace ping
