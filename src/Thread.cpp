/*************************************************************************
* File Name: Thread.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月10日 星期二 09时07分47秒
*************************************************************************/

#include "Util.h"
#include "Thread.h"

namespace ping
{

using std::placeholders::_1;

Thread::Thread(ThreadFunc fun, const string &name)
    : m_threadFunc(fun), m_name(name), m_running(false),
      m_pthreadId(0), m_threadId(0){}

Thread::~Thread()
{
    stop();
}

void Thread::start()
{
    m_running = true;
    CHECK_RETZERO(pthread_create(&m_pthreadId, nullptr, &Thread::run, this));
    m_semaphore.wait(); // 等待一些初始化完成
}

void Thread::stop()
{
    if(m_running)
    {
        m_running = false;
        pthread_join(m_pthreadId, nullptr);
        m_threadId = 0;
        m_pthreadId = 0;
    }
}

void *Thread::run(void *arg)
{
    Thread *thread = static_cast<Thread *>(arg);
    thread->m_threadId = Util::currThreadId();

    t_threadId = thread->m_threadId;
    t_threadName = thread->m_name;
    
    ThreadFunc threadFunc;
    threadFunc.swap(thread->m_threadFunc);

    thread->m_semaphore.notify();
    threadFunc(); 

    return nullptr;
}

}

