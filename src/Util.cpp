/*************************************************************************
* File Name: Util.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 22时23分26秒
*************************************************************************/

#include "Util.h"
#include "Exception.h"

namespace ping
{

thread_local pid_t t_threadId = 0;
thread_local string t_threadName;

pid_t Util::currThreadId()
{
    if(t_threadId <= 0)
    {
        t_threadId = static_cast<pid_t>(::syscall(SYS_gettid));
    }

    return t_threadId;
}

const string Util::currThreadName()
{
    if(t_threadName.empty())
    {
        pid_t threadId = currThreadId();
        t_threadName = "Thread" + std::to_string(threadId);
    }
    return t_threadName;
}

bool Util::isMainThread()
{
    return currThreadId() == ::getpid();
}

Timestamp Util::currTime()
{
    struct timeval tv;
    CHECK_RETZERO(gettimeofday(&tv, nullptr));
    return Timestamp(tv);
}

} // namespace ping
