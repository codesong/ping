/*************************************************************************
* File Name: ThreadPool.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年08月13日 星期五 06时56分48秒
*************************************************************************/

#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <deque>
#include <memory>
#include <vector>
#include <string>
#include "Thread.h"
#include "./Condtion.h"

namespace ping
{

using std::deque;
using std::string;
using std::vector;

class ThreadPool
{
public:
    using ThreadPtr = std::unique_ptr<Thread>;
    using Task = std::function<void ()>;

    ThreadPool(Task initThread, int threadNum, int taskQueueSize = 100, const string &name = "ThreadPool");
    ~ThreadPool();

    void start();
    void stop();

    void execute(Task task);

    const string &name() const { return m_name; } // 不会有竞争，不需加锁
    size_t taskSize() const 
    { 
        MutexGuard _(m_mutex);
        return m_taskQueue.size(); 
    };

private:
    void run();
    void enqueue(Task task);
    Task dequeue();

private:
    enum State
    {
        Stoped,
        Running,
        WaitStoped,
    };

private:
    string  m_name;
    size_t  m_taskSize;
    Task    m_initThread;
    mutable Mutex m_mutex;
    Condtion m_notFull;
    Condtion m_notEmpty;
    atomic<State>   m_state;
    deque<Task>     m_taskQueue;
    vector<ThreadPtr> m_vecThread;
};

}

#endif

