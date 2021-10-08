/*************************************************************************
* File Name: EventLoop.cpp
* Author: codesong
* Mail: codesong@qq.com 
* Created Time: 2021年09月13日 星期一 15时07分43秒
*************************************************************************/

#include "EventLoop.h"
#include "../Timestamp.h"
#include "../Log.h"
#include "Socket.h"
#include <unistd.h>
#include <sys/eventfd.h>

namespace ping
{

const int KPollTimeoutMs = 10000;
thread_local bool t_existEventLoop = false;

EventLoop::EventLoop(bool threadOn, const string &threadName)
    : m_running(false), m_mutex(threadOn ? new Mutex : nullptr), 
      m_thread(threadOn ? new Thread(std::bind(&EventLoop::loop, this), threadName) : nullptr),
      m_wakeupFd(eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)) 
{
    if(m_wakeupFd < 0)
    {
        LOG_FATAL << "eventfd failed";
    }

    m_wakeupChannel = std::make_shared<Channel>(shared_from_this(), m_wakeupFd);

    if(t_existEventLoop)
    {
        LOG_FATAL << "Another EventLoop exists in this thread.";
    }else
    {
        t_existEventLoop = true;
    }
}

EventLoop::~EventLoop()
{
    if(m_running) stop();
    LOG_DEBUG << "EventLoop " << this << " of thread " << m_threadId << " destruct in thread " << Util::currThreadId();
    m_wakeupChannel->disableAll();
    ::close(m_wakeupFd);
    t_existEventLoop = false;
}

void EventLoop::start()
{
    checkThread();
    m_running = true;
    if(m_thread)
    {
        m_thread->start();
    }else
    {
        loop();
    }
}

void EventLoop::stop()
{
    m_running = false;
    if(!inCreatedThread())
    {
        wakeup();
    }
    if(m_thread) m_thread->stop();
}

void EventLoop::loop()
{
    m_threadId = Util::currThreadId();
    while(m_running)
    {
        m_vecActiveChannel.clear();
        Timestamp pollTime = m_poller->poll(KPollTimeoutMs, m_vecActiveChannel);

        for(ChannelPtr channel: m_vecActiveChannel)
        {
            channel->handleEvent(pollTime);
        }
        executeFunctors();
    }
}

void EventLoop::executeFunctors()
{
    vector<Functor> vecFunctor;
    if(m_mutex)
    {
        MutexGuard _(*m_mutex);
        vecFunctor.swap(m_vecFunctor);
    }else
    {
        vecFunctor.swap(m_vecFunctor);
    }

    for(const Functor &functor: vecFunctor)
    {
        functor();
    }
}

void EventLoop::addChannel(ChannelPtr channel)
{
    checkThread();
    m_poller->addChannel(channel);
}

void EventLoop::delChannel(ChannelPtr channel)
{
    checkThread();
    m_poller->delChannel(channel);
}

void EventLoop::updateChannel(ChannelPtr channel)
{
    checkThread();
    m_poller->updateChannel(channel);
}

bool EventLoop::hasChannel(ChannelPtr channel)
{
    checkThread();
    return m_poller->hasChannel(channel);
}

// 可能由其他线程调用
void EventLoop::executeInLoop(Functor cb)
{
    if(inCreatedThread())
    {
        cb();
    }else
    {
        if(m_mutex)
        {
            MutexGuard _(*m_mutex);
            m_vecFunctor.push_back(std::move(cb));
        }else
        {
            m_vecFunctor.push_back(std::move(cb));
        }

        wakeup();
    }
}

void EventLoop::checkThread() 
{
    if(!inCreatedThread())
    {
        LOG_FATAL << "EventLoop was created in thread " << m_threadId 
            << ", current thread " << Util::currThreadId();
    }
}

// eventloop 阻塞在poll上，wakeupFd也通过poll监视，通过wakeup(write wakeupFd)唤醒阻塞在poll的线程
void EventLoop::wakeup() 
{
    uint64_t one = 1;
    ssize_t n = Socket::write(m_wakeupFd, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop wakeup writes failed.";
    }
}

void EventLoop::handleWakeup()
{
    uint64_t one = 1;
    ssize_t n = Socket::read(m_wakeupFd, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR << "EventLoop handleWakeup read failed.";
    }
}

}
