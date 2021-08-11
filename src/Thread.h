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
#include "Mutex.h"

namespace ping
{

using std::string;
using std::atomic;

// libstd++线程设计思路: 
// 1. 构造函数里调用pthread_create，构造即执行线程函数.
// 2. 提供join和detach接口，posix threadid 有效来保证是否join/detach
// 3. 函数结束前必须调用join或者detach,否则抛出异常(现另外提供jthread，线程自动join)
// 提供简单的接口封装，没有按照listd++中的设计封装
// 1. 不利于单独使用线程，例如在log类中使用线程，不能构造log时就运行，就要考虑指针或者智能指针技法，提供start和stop显示启动和停止线程运行
// 2. 灵活接口带来使用的麻烦, 不提供
// 3. 线程析构隐式调用join接口
class Thread: Noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc fun, const string &name = string());
    ~Thread();

    void start();
    void stop();

private:
    void *run(void *arg);

private:
    pid_t           m_threadId;
    string          m_name;
    pthread_t       m_pthreadId;
    Semaphore       m_semaphore; 
    ThreadFunc      m_threadFunc;
    atomic<bool>    m_running;
};

}

#endif

