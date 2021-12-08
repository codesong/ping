/*************************************************************************
* File Name: ThreadPool.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月13日 星期五 07时13分51秒
*************************************************************************/

#include "Exception.h"
#include "ThreadPool.h"

namespace ping
{

ThreadPool::ThreadPool(Task initThread, int threadNum, int taskQueueSize, const string &name)
    : m_initThread(initThread), m_taskSize(taskQueueSize), m_name(name), 
      m_state(Stoped), m_mutex(), m_notFull(m_mutex), m_notEmpty(m_mutex)
{
    if(taskQueueSize <= 0)
    {
        throw std::logic_error("taskQueue size must greater than zero.");
    }
    if(threadNum > 0)
    {
        m_vecThread.reserve(threadNum);
        for(int i = 0; i < threadNum; ++i)
        {
            m_vecThread.emplace_back(std::make_unique<Thread>(
                std::bind(&ThreadPool::run, this), m_name + std::to_string(i+1)));
        }
    }
}

ThreadPool::~ThreadPool()
{
    if(Stoped != m_state)
        stop();
}

void ThreadPool::start()
{
    m_state = Running;
    if(!m_vecThread.empty())
    {
        for(ThreadPtr &thread: m_vecThread)
        {
            thread->start();
        }
    }else
    {
        if(m_initThread) m_initThread();
    }
}

void ThreadPool::stop()
{
    MutexGuard _(m_mutex);
    m_notFull.notifyAll();
    m_notEmpty.notifyAll();
    if(Running == m_state)
    {
        if(m_taskQueue.empty())
        {
            m_state = Stoped;
        }else
        {
            m_state = WaitStoped;
        }
    }else if(WaitStoped == m_state)
    {
        if(m_taskQueue.empty())
            m_state = Stoped;
    }

    if(Stoped == m_state)
    {
        for(ThreadPtr &thread: m_vecThread)
        {
            thread->stop();
        }
        m_vecThread.clear();
    }
}

void ThreadPool::execute(Task task)
{
    if(Running == m_state) // 运行状态才接受新任务
    {
        if(m_vecThread.empty())    
        {
            task();
        }else
        {
            enqueue(task);
        }
    }
}

void ThreadPool::enqueue(Task task)
{
    MutexGuard _(m_mutex);
    while(Running == m_state && m_taskQueue.size() >= m_taskSize)
    {
        m_notFull.wait();
    }
    if(Running != m_state) return;

    m_taskQueue.push_back(std::move(task));
    m_notEmpty.notify();
}

ThreadPool::Task ThreadPool::dequeue()
{
    MutexGuard _(m_mutex);
    while(Stoped != m_state && m_taskQueue.empty())
    {
        m_notEmpty.wait();
    }
    Task task;
    if(!m_taskQueue.empty())
    {
        task = m_taskQueue.front();
        m_taskQueue.pop_front();
        m_notFull.notify();
    }
    return task;
}

void ThreadPool::run()
{
    try
    {
        if(m_initThread) { m_initThread(); }

        while(Stoped != m_state)
        {
            Task task = dequeue();
            if(task)
            {
                task();
            }

            // 处理完现有任务后退出线程
            if(WaitStoped == m_state && m_taskQueue.empty())
            {
                stop(); 
            }
        }
    }catch(const Exception &ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", m_name.c_str())        ;
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackInfo());
        throw;
    }catch(const std::exception &ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", m_name.c_str())        ;
        fprintf(stderr, "reason: %s\n", ex.what());
        throw;
    }catch(...)
    {
        fprintf(stderr, "unknow exception caught in ThreadPool %s\n", m_name.c_str())        ;
        throw;
    }
}

}
