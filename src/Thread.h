/*************************************************************************
* File Name: Thread.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 04时49分02秒
*************************************************************************/

#ifndef __THREAD_H__
#define __THREAD_H__

#include <atomic>
#include <string>
#include <functional>

#include <pthread.h>

namespace ping
{

using std::string;
using std::atomic;

class Thread: Noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc fun, const string &name = string())

private:
    atomic<bool> m_running;
    pid_t m_threadId;
    pthread_t m_pthreadId;
    ThreadFunc m_threadFunc;
    string m_name;


};

}

#endif

