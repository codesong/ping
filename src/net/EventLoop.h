/*************************************************************************
* File Name: EventLoop.h
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分22秒
*************************************************************************/

#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <vector>
#include <memory>
#include "Poller.h"
#include "Channel.h"
#include "../Util.h"
#include "../Mutex.h"
#include "../Thread.h"

namespace ping
{
using std::vector;

class EventLoop: Noncopyable
{
public:
    using Functor = std::function<void()>;
    using PollerPtr = std::unique_ptr<Poller>;
    using ChannelPtr = std::unique_ptr<Channel>;

public:
    EventLoop();
    ~EventLoop();

    void start();
    void stop();

    void addChannel(Channel *channel);
    void delChannel(Channel *channel);
    void updateChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    void runInLoop(Functor cb);
    bool inLoopThread();
    void checkThread();

private:
    void loop();
    void wakeup();
    void handleWakeup();

    void runFunctors();

private:
    const int m_wakeupFd;
    pid_t m_threadId;
    Mutex  m_mutex;
    PollerPtr m_poller; 
    atomic<bool> m_running;
    ChannelPtr m_wakeupChannel; 
    vector<Functor> m_vecFunctor;
    vector<Channel *> m_vecActiveChannel;
};

using EventLoopPtr = std::shared_ptr<EventLoop>;
class EventLoopThread: Noncopyable
{
public:
    EventLoopThread(const string name = "");
    ~EventLoopThread();
    
    EventLoopPtr eventLoop(){ return m_eventLoop; }

private:
    void threadFunc();

private:
    Thread m_thread;
    EventLoopPtr m_eventLoop;
};

}

#endif

